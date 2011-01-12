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
	JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_init(JNIEnv *env, jclass obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_set_1asset(JNIEnv *env, jclass obj, jstring apk);
	JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_step(JNIEnv *env, jclass obj, jlong ms);
}
#endif

JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_init(JNIEnv *env, jclass obj, jint width, jint height)
{
	static bool initialised = false;

	if (!initialised)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);

		initialised = true;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof(0, width, 0, height, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, width, height);

	LOGI("Initialise screen with %i x %i", width, height);
	Screen::Instance()->init(width, height);
	LOGI("Initialised screen");
}

JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_set_1asset(JNIEnv *env, jclass obj, jstring apk)
{
	const char *path = env->GetStringUTFChars(apk, 0);
	AssetManager::Instance()->set_source(path);

	LOGI("Now loading assets-hd.png from %s...", path);
	unsigned char *tmp;
	AssetManager::Instance()->load(tmp, "assets-hd.png");
	LOGI("Loaded assets-hd.png ok");

    env->ReleaseStringUTFChars(apk, path);
}

JNIEXPORT void JNICALL Java_com_bifrostgames_onwire_OnWire_step(JNIEnv *env, jclass obj, jlong ms)
{
	static OnWireGame the_game;

	// Update game logic
	Physics::Instance()->step(ms / 1000.0f);
	the_game.update();

	// Draw
	glClear(GL_COLOR_BUFFER_BIT);
	the_game.draw();
}
