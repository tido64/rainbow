// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_ANDROID

#include <memory>
#include <unistd.h>
#include <sys/types.h>

#include <EGL/egl.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "Config.h"
#include "Director.h"
#include "Common/Chrono.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "FileSystem/Path.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Input/Touch.h"

bool active = false;  ///< Whether the window is in focus.
bool done = false;    ///< Whether the user has requested to quit.
ANativeActivity *gNativeActivity;

struct AInstance
{
	bool initialised;
	uint32_t width;
	uint32_t height;

	ASensorManager *sensorManager;
	ASensorEventQueue *sensorEventQueue;
	const ASensor *accelerometerSensor;
	struct android_app *app;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	std::unique_ptr<Director> director;
	ConFuoco::Mixer mixer;

	AInstance()
	    : initialised(false), width(0), height(0), sensorManager(nullptr),
	      sensorEventQueue(nullptr), accelerometerSensor(nullptr), app(nullptr),
	      display(EGL_NO_DISPLAY), surface(EGL_NO_SURFACE),
	      context(EGL_NO_CONTEXT) { }
};

void android_destroy_display(AInstance *ainstance);
void android_handle_display(AInstance *ainstance);
void android_init_display(AInstance *ainstance);
void android_handle_event(struct android_app *app, int32_t cmd);
int32_t android_handle_input(struct android_app *app, AInputEvent *event);
int32_t android_handle_motion(struct android_app *app, AInputEvent *event);
Touch get_touch_event(AInstance *ainstance,
                      AInputEvent *event,
                      const int32_t index);
Touch get_touch_event(AInstance *ainstance,
                      AInputEvent *event,
                      const int32_t index,
                      const size_t history);


void android_main(struct android_app *state)
{
	app_dummy();
	//sleep(5);  // Sleep a little so GDB can attach itself

	AInstance ainstance;
	state->userData = &ainstance;
	state->onAppCmd = android_handle_event;
	state->onInputEvent = android_handle_input;
	ainstance.app = state;
	gNativeActivity = state->activity;
	Path::set_current();

	// Prepare to monitor accelerometer
	ainstance.sensorManager = ASensorManager_getInstance();
	ainstance.accelerometerSensor = ASensorManager_getDefaultSensor(ainstance.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	if (ainstance.accelerometerSensor)
		ainstance.sensorEventQueue = ASensorManager_createEventQueue(ainstance.sensorManager, state->looper, LOOPER_ID_USER, nullptr, nullptr);

	Chrono chrono;
	while (!done)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!active ? -1 : 0, nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
		{
			if (source)
				source->process(state, source);

			if (done)
				break;

			// If a sensor has data, process it now.
			if (active && ident == LOOPER_ID_USER && ainstance.accelerometerSensor)
			{
				ASensorEvent event;
				while (ASensorEventQueue_getEvents(ainstance.sensorEventQueue, &event, 1) > 0)
					Input::Instance->accelerated(
						event.acceleration.x,
						event.acceleration.y,
						event.acceleration.z,
						event.timestamp);
			}
		}

		chrono.update();
		if (!(ainstance.initialised & active))
			continue;

		ainstance.director->update(chrono.delta());

		Renderer::clear();
		ainstance.director->draw();
		eglSwapBuffers(ainstance.display, ainstance.surface);
	}
	android_destroy_display(&ainstance);
}

void android_destroy_display(AInstance *a)
{
	a->director.reset();
	if (a->display != EGL_NO_DISPLAY)
	{
		Renderer::release();
		eglMakeCurrent(a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (a->context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(a->display, a->context);
			a->context = EGL_NO_CONTEXT;
		}
		if (a->surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(a->display, a->surface);
			a->surface = EGL_NO_SURFACE;
		}
		eglTerminate(a->display);
		a->display = EGL_NO_DISPLAY;
	}
	a->initialised = false;
	active = false;
}

void android_handle_display(AInstance *a)
{
	if (a->initialised)
		return;

	if (!Renderer::init())
	{
		R_ERROR("[Rainbow] Failed to initialise renderer");
		done = true;
		return;
	}

	EGLint value;
	eglQuerySurface(a->display, a->surface, EGL_WIDTH, &value);
	a->width = value;
	eglQuerySurface(a->display, a->surface, EGL_HEIGHT, &value);
	a->height = value;
	Renderer::resize(a->width, a->height);

	// Load game
	a->director.reset(new Director());
	a->director->init(Data::load_asset("main.lua"), a->width, a->height);

	a->initialised = true;
}

void android_init_display(AInstance *a)
{
	EGLDisplay &dpy = a->display;
	if (dpy == EGL_NO_DISPLAY)
	{
		dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		eglInitialize(dpy, 0, 0);
	}

	const EGLint attrib_list[] = {
		EGL_ALPHA_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	EGLConfig config;
	EGLint nconfigs;
	eglChooseConfig(dpy, attrib_list, &config, 1, &nconfigs);

	EGLSurface &surface = a->surface;
	if (surface == EGL_NO_SURFACE)
	{
		EGLint format;
		eglGetConfigAttrib(dpy, config, EGL_NATIVE_VISUAL_ID, &format);
		ANativeWindow_setBuffersGeometry(a->app->window, 0, 0, format);

		surface = eglCreateWindowSurface(dpy, config, a->app->window, nullptr);
		if (surface == EGL_NO_SURFACE)
		{
			R_ERROR("[Rainbow] Failed to create EGL window surface");
			return;
		}
	}

	EGLContext &ctx = a->context;
	if (ctx == EGL_NO_CONTEXT)
	{
		const EGLint gles_attrib[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
		ctx = eglCreateContext(dpy, config, EGL_NO_CONTEXT, gles_attrib);
		if (ctx == EGL_NO_CONTEXT)
		{
			R_ERROR("[Rainbow] Failed to create EGL rendering context");
			return;
		}
	}

	done = eglMakeCurrent(dpy, surface, surface, ctx) == EGL_FALSE;
	if (done)
		R_ERROR("[Rainbow] Failed to bind context to the current rendering thread");
	else
		android_handle_display(a);
}

void android_handle_event(struct android_app *app, int32_t cmd)
{
	AInstance* a = static_cast<AInstance*>(app->userData);
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			android_init_display(a);
			break;
		case APP_CMD_TERM_WINDOW:
			eglMakeCurrent(a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			eglDestroySurface(a->display, a->surface);
			a->surface = EGL_NO_SURFACE;
			break;
		case APP_CMD_WINDOW_RESIZED:
			android_handle_display(a);
			break;
		case APP_CMD_GAINED_FOCUS:
			// When our app gains focus, we start monitoring the accelerometer.
			if (a->accelerometerSensor)
			{
				ASensorEventQueue_enableSensor(a->sensorEventQueue, a->accelerometerSensor);
				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate(a->sensorEventQueue, a->accelerometerSensor, (1000L / 60) * 1000);
			}
			ConFuoco::Mixer::Instance->suspend(false);
			active = true;
			break;
		case APP_CMD_LOST_FOCUS:
			active = false;
			break;
		case APP_CMD_LOW_MEMORY:
			a->director->on_memory_warning();
			break;
		case APP_CMD_RESUME:
			if (a->surface)
			{
				eglMakeCurrent(a->display, a->surface, a->surface, a->context);

				// From NVIDIA:
				// Note that in some cases, only the onPause is received on suspend, in which case, the
				// focus lost callback actually comes when the device is resumed to the lock screen. In
				// other words, resuming sends an indication that you have been resumed (onResume)
				// and then an indication that you are hidden (focus lost). And the unlock case remains
				// unchanged (focus regained).
				if (active)
					android_handle_event(app, APP_CMD_GAINED_FOCUS);
			}
			break;
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_PAUSE:
			eglMakeCurrent(a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			if (a->accelerometerSensor)
				ASensorEventQueue_disableSensor(a->sensorEventQueue, a->accelerometerSensor);
			ConFuoco::Mixer::Instance->suspend(true);
			break;
		case APP_CMD_DESTROY:
			active = false;
			done = true;
			break;
		default:
			break;
	}
}

int32_t android_handle_input(struct android_app *app, AInputEvent *event)
{
	switch (AInputEvent_getType(event))
	{
		case AINPUT_EVENT_TYPE_KEY:
			return AKeyEvent_getKeyCode(event) == AKEYCODE_BACK;
		case AINPUT_EVENT_TYPE_MOTION:
			return android_handle_motion(app, event);
		default:
			return 0;
	}
}

int32_t android_handle_motion(struct android_app *app, AInputEvent *event)
{
	AInstance* a = static_cast<AInstance*>(app->userData);
	switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK)
	{
		case AMOTION_EVENT_ACTION_DOWN:
		case AMOTION_EVENT_ACTION_POINTER_DOWN:
			{
				const int32_t index
						= (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
						>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
				Touch t = get_touch_event(a, event, index);
				Input::Instance->touch_began(&t, 1);
			}
			break;
		case AMOTION_EVENT_ACTION_UP:
		case AMOTION_EVENT_ACTION_POINTER_UP:
			{
				const int32_t index
						= (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
						>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
				Touch t = get_touch_event(a, event, index);
				Input::Instance->touch_ended(&t, 1);
			}
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			{
				size_t history = AMotionEvent_getHistorySize(event);
				if (!history)
					break;
				--history;

				const size_t count = AMotionEvent_getPointerCount(event);
				std::unique_ptr<Touch[]> touches(new Touch[count]);

				for (size_t i = 0; i < count; ++i)
					touches[i] = get_touch_event(a, event, i, history);
				Input::Instance->touch_moved(touches.get(), count);
			}
			break;
		case AMOTION_EVENT_ACTION_CANCEL:
		case AMOTION_EVENT_ACTION_OUTSIDE:
			//Input::Instance->touch_canceled(touches, count);
			Input::Instance->touch_canceled();
			break;
		default:
			break;
	}
	return 1;
}

Touch get_touch_event(AInstance *a, AInputEvent *event, const int32_t index)
{
	return Touch(AMotionEvent_getPointerId(event, index),
	             AMotionEvent_getX(event, index),
	             a->height - AMotionEvent_getY(event, index),
	             AMotionEvent_getEventTime(event));
}

Touch get_touch_event(AInstance *a,
                      AInputEvent *event,
                      const int32_t index,
                      const size_t history)
{
	Touch t = get_touch_event(a, event, index);
	t.x0 = AMotionEvent_getHistoricalX(event, index, history);
	t.y0 = a->height - AMotionEvent_getHistoricalY(event, index, history);
	return t;
}

#endif
