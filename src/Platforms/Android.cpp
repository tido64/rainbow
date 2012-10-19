// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include <unistd.h>
#include <sys/types.h>
#include <EGL/egl.h>
#include <android/asset_manager.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "Config.h"
#include "Director.h"
#include "Common/Chrono.h"
#include "Common/Data.h"
#include "ConFuoco/Mixer.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Input/Touch.h"

bool active = false;  ///< Whether the window is in focus.
bool done = false;    ///< Whether the user has requested to quit.
struct AAssetManager *g_asset_manager;

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
	Director director;

	AInstance() :
		initialised(false), width(0), height(0), sensorManager(nullptr),
		sensorEventQueue(nullptr), accelerometerSensor(nullptr),
		app(nullptr) { }
};

void android_destroy_display(AInstance *ainstance);
void android_init_display(AInstance *ainstance);
void android_handle_event(struct android_app *app, int32_t cmd);
int32_t android_handle_input(struct android_app *app, AInputEvent *event);
int32_t android_handle_motion(struct android_app *app, AInputEvent *event);


void android_main(struct android_app *state)
{
	app_dummy();
	//sleep(5);  // Sleep a little so GDB can attach itself

	AInstance ainstance;

	state->userData = &ainstance;
	state->onAppCmd = android_handle_event;
	state->onInputEvent = android_handle_input;
	ainstance.app = state;
	g_asset_manager = state->activity->assetManager;

	// Prepare to monitor accelerometer
	ainstance.sensorManager = ASensorManager_getInstance();
	ainstance.accelerometerSensor = ASensorManager_getDefaultSensor(ainstance.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
	if (ainstance.accelerometerSensor)
		ainstance.sensorEventQueue = ASensorManager_createEventQueue(ainstance.sensorManager, state->looper, LOOPER_ID_USER, nullptr, nullptr);

	Chrono::Instance().update();
	while (!done)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident = ALooper_pollAll(!active ? -1 : 0, nullptr, &events, reinterpret_cast<void**>(&source))) >= 0)
		{
			if (source)
				source->process(state, source);

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

		if (!(ainstance.initialised & active))
		{
			Chrono::Instance().update();
			continue;
		}

		ainstance.director.update();

		Renderer::clear();
		ainstance.director.draw();
		eglSwapBuffers(ainstance.display, ainstance.surface);
	}
	android_destroy_display(&ainstance);
	ANativeActivity_finish(state->activity);
}

void android_destroy_display(AInstance *ainstance)
{
	if (ainstance->display != EGL_NO_DISPLAY)
	{
		Renderer::release();
		eglMakeCurrent(ainstance->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (ainstance->context != EGL_NO_CONTEXT)
		{
			eglDestroyContext(ainstance->display, ainstance->context);
			ainstance->context = EGL_NO_CONTEXT;
		}
		if (ainstance->surface != EGL_NO_SURFACE)
		{
			eglDestroySurface(ainstance->display, ainstance->surface);
			ainstance->surface = EGL_NO_SURFACE;
		}
		eglTerminate(ainstance->display);
		ainstance->display = EGL_NO_DISPLAY;
	}
	ainstance->initialised = false;
	active = false;
}

void android_init_display(AInstance *ainstance)
{
	EGLDisplay &dpy = ainstance->display;
	EGLSurface &surface = ainstance->surface;
	EGLContext &ctx = ainstance->context;

	dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(dpy, 0, 0);

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

	EGLint format;
	eglGetConfigAttrib(dpy, config, EGL_NATIVE_VISUAL_ID, &format);
	ANativeWindow_setBuffersGeometry(ainstance->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(dpy, config, ainstance->app->window, nullptr);
	if (surface == EGL_NO_SURFACE)
	{
		R_ERROR("[Rainbow] Failed to create EGL window surface");
		return;
	}

	const EGLint gles_attrib[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	ctx = eglCreateContext(dpy, config, EGL_NO_CONTEXT, gles_attrib);
	if (ctx == EGL_NO_CONTEXT)
	{
		R_ERROR("[Rainbow] Failed to create EGL rendering context");
		return;
	}

	done = eglMakeCurrent(dpy, surface, surface, ctx) == EGL_FALSE;
	if (done)
	{
		R_ERROR("[Rainbow] Failed to bind context to the current rendering thread");
		return;
	}

	if (!Renderer::init())
	{
		R_ERROR("[Rainbow] Failed to initialise renderer");
		done = true;
		return;
	}

	EGLint value;
	eglQuerySurface(dpy, surface, EGL_WIDTH, &value);
	ainstance->width = value;
	eglQuerySurface(dpy, surface, EGL_HEIGHT, &value);
	ainstance->height = value;
	Renderer::resize(ainstance->width, ainstance->height);
	ainstance->director.set_video(ainstance->width, ainstance->height);

	// Load game
	Data main("main.lua");
	R_ASSERT(main, "Failed to load 'main'");
	ainstance->director.init(main);

	ainstance->initialised = true;
}

void android_handle_event(struct android_app *app, int32_t cmd)
{
	AInstance* ainstance = static_cast<AInstance*>(app->userData);
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			android_init_display(ainstance);
			break;
		case APP_CMD_TERM_WINDOW:
			android_destroy_display(ainstance);
			break;
		case APP_CMD_GAINED_FOCUS:
			// When our app gains focus, we start monitoring the accelerometer.
			if (ainstance->accelerometerSensor)
			{
				ASensorEventQueue_enableSensor(ainstance->sensorEventQueue, ainstance->accelerometerSensor);
				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate(ainstance->sensorEventQueue, ainstance->accelerometerSensor, (1000L / 60) * 1000);
			}
			active = true;
			break;
		case APP_CMD_LOST_FOCUS:
			// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
			if (ainstance->accelerometerSensor)
				ASensorEventQueue_disableSensor(ainstance->sensorEventQueue, ainstance->accelerometerSensor);
			Renderer::clear();
			active = false;
			break;
		case APP_CMD_LOW_MEMORY:
			ainstance->director.on_memory_warning();
			break;
		case APP_CMD_SAVE_STATE:
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
			return 0;
		case AINPUT_EVENT_TYPE_MOTION:
			return android_handle_motion(app, event);
		default:
			return 0;
	}
}

int32_t android_handle_motion(struct android_app *app, AInputEvent *event)
{
	const int32_t action = AMotionEvent_getAction(event);
	if (action == AMOTION_EVENT_ACTION_POINTER_DOWN || action == AMOTION_EVENT_ACTION_POINTER_UP)
		return 0;

	const uint32_t height = static_cast<AInstance*>(app->userData)->height;
	const size_t count = AMotionEvent_getPointerCount(event);
	Touch *touches = new Touch[count];
	for (size_t i = 0; i < count; ++i)
	{
		touches[i].hash = AMotionEvent_getPointerId(event, i);
		touches[i].x    = AMotionEvent_getX(event, i);
		touches[i].y    = height - AMotionEvent_getY(event, i);
		touches[i].x0   = AMotionEvent_getHistoricalX(event, i, 0);
		touches[i].y0   = height - AMotionEvent_getHistoricalY(event, i, 0);
	}
	switch (action)
	{
		case AMOTION_EVENT_ACTION_DOWN:
			Input::Instance->touch_began(touches, count);
			break;
		case AMOTION_EVENT_ACTION_UP:
			Input::Instance->touch_ended(touches, count);
			break;
		case AMOTION_EVENT_ACTION_MOVE:
			Input::Instance->touch_moved(touches, count);
			break;
		case AMOTION_EVENT_ACTION_CANCEL:
		case AMOTION_EVENT_ACTION_OUTSIDE:
			//Input::Instance->touch_canceled(touches, count);
			Input::Instance->touch_canceled();
			break;
		default:
			break;
	}
	delete[] touches;
	return 1;
}

#endif
