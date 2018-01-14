// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#include <EGL/egl.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Director.h"
#include "Input/Pointer.h"

using rainbow::Chrono;
using rainbow::Pointer;
using rainbow::Vec2i;

ANativeActivity* g_native_activity;

struct AInstance
{
    bool active;  ///< Whether the window is in focus.
    bool done;    ///< Whether the user has requested to quit.
    bool initialised;

    struct android_app* app;
    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    std::unique_ptr<Director> director;

    AInstance()
        : active(false), done(false), initialised(false), app(nullptr),
          sensorManager(nullptr), accelerometerSensor(nullptr),
          sensorEventQueue(nullptr), display(EGL_NO_DISPLAY),
          surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT) {}
};

void android_destroy_display(AInstance*);
void android_handle_display(AInstance*);
void android_init_display(AInstance*);
void android_handle_event(struct android_app*, int32_t cmd);
auto android_handle_input(struct android_app*, AInputEvent*) -> int32_t;
auto android_handle_motion(struct android_app*, AInputEvent*) -> int32_t;
auto get_pointer_event(AInputEvent*, int32_t index) -> Pointer;

namespace
{
    pthread_key_t s_thread_key;

    void detach_current_thread(void* vm)
    {
        if (vm == nullptr)
            return;

        static_cast<JavaVM*>(vm)->DetachCurrentThread();
        pthread_setspecific(s_thread_key, nullptr);
    }
}

void android_main(struct android_app* state)
{
    pthread_key_create(&s_thread_key, detach_current_thread);

    JNIEnv* env;
    state->activity->vm->AttachCurrentThread(&env, nullptr);
    pthread_setspecific(s_thread_key, state->activity->vm);

    AInstance ainstance;
    state->userData = &ainstance;
    state->onAppCmd = android_handle_event;
    state->onInputEvent = android_handle_input;
    ainstance.app = state;
    g_native_activity = state->activity;

    // Prepare to monitor accelerometer
    ainstance.sensorManager = ASensorManager_getInstance();
    ainstance.accelerometerSensor = ASensorManager_getDefaultSensor(
        ainstance.sensorManager, ASENSOR_TYPE_ACCELEROMETER);
    if (ainstance.accelerometerSensor)
    {
        ainstance.sensorEventQueue = ASensorManager_createEventQueue(
            ainstance.sensorManager, state->looper, LOOPER_ID_USER, nullptr,
            nullptr);
    }

    Chrono chrono;
    while (!ainstance.done)
    {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(
                    (!ainstance.active ? -1 : 0), nullptr, &events,
                    reinterpret_cast<void**>(&source))) >= 0)
        {
            if (source)
                source->process(state, source);

            if (ainstance.done)
                break;

            // If a sensor has data, process it now.
            if (ainstance.active && ident == LOOPER_ID_USER &&
                ainstance.accelerometerSensor)
            {
                ASensorEvent event;
                while (ASensorEventQueue_getEvents(
                           ainstance.sensorEventQueue, &event, 1) > 0)
                {
                    ainstance.director->input().accelerated(
                        event.acceleration.x,
                        event.acceleration.y,
                        event.acceleration.z,
                        event.timestamp);
                }
            }
        }

        chrono.tick();
        if (!(ainstance.initialised & ainstance.active))
            continue;

        ainstance.director->update(chrono.delta());
        ainstance.director->draw();
        eglSwapBuffers(ainstance.display, ainstance.surface);
    }

    android_destroy_display(&ainstance);
}

void android_destroy_display(AInstance* a)
{
    a->director.reset();
    if (a->display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(
            a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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
    a->active = false;
}

void android_handle_display(AInstance* a)
{
    if (a->initialised)
        return;

    a->director = std::make_unique<Director>();
    if (a->director->terminated())
    {
        LOGF("%s", a->director->error());
        a->done = true;
        return;
    }

    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(a->display, a->surface, EGL_WIDTH, &width);
    eglQuerySurface(a->display, a->surface, EGL_HEIGHT, &height);

    // Load game
    a->director->init(Vec2i{width, height});
    a->initialised = !a->director->terminated();
}

void android_init_display(AInstance* a)
{
    EGLDisplay& dpy = a->display;
    if (dpy == EGL_NO_DISPLAY)
    {
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(dpy, 0, 0);
    }

    const EGLint attrib_list[]{
        EGL_ALPHA_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE};
    EGLConfig config;
    EGLint nconfigs;
    eglChooseConfig(dpy, attrib_list, &config, 1, &nconfigs);

    EGLSurface& surface = a->surface;
    if (surface == EGL_NO_SURFACE)
    {
        EGLint format;
        eglGetConfigAttrib(dpy, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(a->app->window, 0, 0, format);

        surface = eglCreateWindowSurface(dpy, config, a->app->window, nullptr);
        if (surface == EGL_NO_SURFACE)
        {
            LOGF("Failed to create EGL window surface");
            return;
        }
    }

    EGLContext& ctx = a->context;
    if (ctx == EGL_NO_CONTEXT)
    {
        const EGLint gles_attrib[]{EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        ctx = eglCreateContext(dpy, config, EGL_NO_CONTEXT, gles_attrib);
        if (ctx == EGL_NO_CONTEXT)
        {
            LOGF("Failed to create EGL rendering context");
            return;
        }
    }

    a->done = eglMakeCurrent(dpy, surface, surface, ctx) == EGL_FALSE;
    if (a->done)
        LOGF("Failed to bind context to the current rendering thread");
    else
        android_handle_display(a);
}

void android_handle_event(struct android_app* app, int32_t cmd)
{
    AInstance* a = static_cast<AInstance*>(app->userData);
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
            android_init_display(a);
            break;
        case APP_CMD_TERM_WINDOW:
            eglMakeCurrent(
                a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
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
                ASensorEventQueue_enableSensor(
                    a->sensorEventQueue, a->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(
                    a->sensorEventQueue, a->accelerometerSensor,
                    (1000L / 60) * 1000);
            }
            a->director->on_focus_gained();
            a->active = true;
            break;
        case APP_CMD_LOST_FOCUS:
            a->active = false;
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
                if (a->active)
                    android_handle_event(app, APP_CMD_GAINED_FOCUS);
            }
            break;
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_PAUSE:
            eglMakeCurrent(
                a->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            if (a->accelerometerSensor)
            {
                ASensorEventQueue_disableSensor(
                    a->sensorEventQueue, a->accelerometerSensor);
            }
            a->director->on_focus_lost();
            break;
        case APP_CMD_DESTROY:
            a->active = false;
            a->done = true;
            break;
        default:
            break;
    }
}

auto android_handle_input(struct android_app* app, AInputEvent* event)
    -> int32_t
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

auto android_handle_motion(struct android_app* app, AInputEvent* event)
    -> int32_t
{
    Director* director = static_cast<AInstance*>(app->userData)->director.get();
    switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK)
    {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN: {
            const int32_t index = (AMotionEvent_getAction(event) &
                                   AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            Pointer p = get_pointer_event(event, index);
            director->input().on_pointer_began(ArrayView<Pointer>{p});
            break;
        }
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP: {
            const int32_t index = (AMotionEvent_getAction(event) &
                                   AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            Pointer p = get_pointer_event(event, index);
            director->input().on_pointer_ended(ArrayView<Pointer>{p});
            break;
        }
        case AMOTION_EVENT_ACTION_MOVE: {
            const size_t count = AMotionEvent_getPointerCount(event);
            auto pointers = std::make_unique<Pointer[]>(count);
            for (size_t i = 0; i < count; ++i)
                pointers[i] = get_pointer_event(event, i);
            director->input().on_pointer_moved(
                ArrayView<Pointer>{pointers.get(), count});
            break;
        }
        case AMOTION_EVENT_ACTION_CANCEL:
        case AMOTION_EVENT_ACTION_OUTSIDE:
            director->input().on_pointer_canceled();
            break;
        default:
            break;
    }
    return 1;
}

auto get_pointer_event(AInputEvent* event, int32_t index) -> Pointer
{
    const Vec2i& point = rainbow::graphics::convert_to_flipped_view(Vec2i(
        AMotionEvent_getX(event, index), AMotionEvent_getY(event, index)));
    return {static_cast<uint32_t>(AMotionEvent_getPointerId(event, index)),
            point.x,
            point.y,
            static_cast<uint64_t>(AMotionEvent_getEventTime(event))};
}
