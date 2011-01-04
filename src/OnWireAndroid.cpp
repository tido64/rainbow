/// Android native application glue code.

/// Seems like the application crashes on some phones (or at least on my Hero),
/// so we're NOT using NativeActivity.
///
/// \see http://blog.jayway.com/2009/12/03/opengl-es-tutorial-for-android-part-i/
/// \see http://developer.android.com/reference/android/app/NativeActivity.html
/// \see http://www.philhassey.com/blog/2010/07/22/android-day-3-packaging-assets-jni-and-opengl/
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include <jni.h>

#include <android/log.h>
//#include <android/sensor.h>
//#include <android_native_app_glue.h>

//#include <EGL/egl.h>
#include <GLES/gl.h>

#include "OnWireGame.h"

#define LOG_TAG "onwire"
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#define DEBUG 1

#ifdef __cplusplus
extern "C"
{
	JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_init(JNIEnv * env, jobject obj,  jint width, jint height);
	JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_step(JNIEnv * env, jobject obj);
}
#endif

JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_init(JNIEnv * env, jobject obj, jint width, jint height)
{
	// Initialize 2d viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, width, height, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, width, height);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	Screen::init(width, height);
}

JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_step(JNIEnv * env, jobject obj)
{
	// perform update (should be in a separate thread)
	// draw


	glClear(GL_COLOR_BUFFER_BIT);
}

/** NativeActivity code

struct saved_state
{
	float angle;
	int32_t x;
	int32_t y;
};

struct engine
{
	struct android_app *app;

	ASensorManager *sensor_manager;
	const ASensor *accelerometer_sensor;
	ASensorEventQueue *sensor_event_queue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};

static void engine_draw_frame(struct engine *engine);
static void engine_handle_cmd(struct android_app* app, int32_t cmd);
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event);
static int engine_init_display(struct engine *engine);
static void engine_term_display(struct engine *engine);

void android_main(struct android_app *state)
{
	struct engine engine;

	app_dummy();  // Make sure glue isn't stripped

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	engine.sensor_manager = ASensorManager_getInstance();
	engine.accelerometer_sensor = ASensorManager_getDefaultSensor(
		engine.sensor_manager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensor_event_queue = ASensorManager_createEventQueue(
		engine.sensor_manager,
		state->looper,
		LOOPER_ID_USER,
		0,
		0);

	if (state->savedState != 0)
		engine.state = *(struct saved_state*)state->savedState;

	while (1)
	{
		int ident;
		int events;
		struct android_poll_source *source;

		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, 0, &events, (void **)&source)) >= 0)
		{
			if (source != 0)
				source->process(state, source);

			if (ident == LOOPER_ID_USER)
			{
				if (engine.accelerometer_sensor != 0)
				{
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensor_event_queue, &event, 1) > 0)
					{
						LOGI("Accelerometer: x=%f y=%f z=%f",
							event.acceleration.x,
							event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			if (state->destroyRequested != 0)
			{
				LOGI("Engine thread destroy requested!");
				engine_term_display(&engine);
				return;
			}

			if (engine.animating)
			{
				engine.state.angle += 0.01f;
				if (engine.state.angle > 1)
					engine.state.angle = 0;

				engine_draw_frame(&engine);
			}
		}
	}
}

static void engine_draw_frame(struct engine *engine)
{
	if (engine->display == 0) return;

	glClearColor(((float)engine->state.x)/engine->width, engine->state.angle,
		((float)engine->state.y)/engine->height, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(engine->display, engine->surface);
}

static int engine_init_display(struct engine *engine)
{
	const EGLint attribs[] = {
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_DEPTH_SIZE, 16,
		//EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint num_configs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, 0, 0);
	eglChooseConfig(display, attribs, &config, 1, &num_configs);
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, 0);
	context = eglCreateContext(display, config, 0, 0);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
	{
		LOGE("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;


	// Initialize 2d viewport
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, h, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, w, h);

	glPushAttrib(GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	return 0;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
			engine->app->savedState = malloc(sizeof(struct saved_state));
			*((struct saved_state*)engine->app->savedState) = engine->state;
			engine->app->savedStateSize = sizeof(struct saved_state);
			break;
		case APP_CMD_INIT_WINDOW:
			if (engine->app->window != 0)
			{
				engine_init_display(engine);
				engine_draw_frame(engine);
			}
			break;
		case APP_CMD_TERM_WINDOW:
			engine_term_display(engine);
			break;
		case APP_CMD_GAINED_FOCUS:
			if (engine->accelerometer_sensor != 0)
			{
				ASensorEventQueue_enableSensor(engine->sensor_event_queue, engine->accelerometer_sensor);
				ASensorEventQueue_setEventRate(engine->sensor_event_queue, engine->accelerometer_sensor, (1000L / 60) * 1000);
			}
			break;
		case APP_CMD_LOST_FOCUS:
			if (engine->accelerometer_sensor != 0)
				ASensorEventQueue_disableSensor(engine->sensor_event_queue, engine->accelerometer_sensor);
			engine->animating = 0;
			engine_draw_frame(engine);
			break;
	}
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event)
{
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		engine->animating = 1;
		engine->state.x = AMotionEvent_getX(event, 0);
		engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

static void engine_term_display(struct engine *engine)
{
	if (engine->display != EGL_NO_DISPLAY)
	{
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT)
			eglDestroyContext(engine->display, engine->context);
		if (engine->surface != EGL_NO_SURFACE)
			eglDestroySurface(engine->display, engine->surface);
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}
*/
