// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <optional>

#include <EGL/egl.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android_native_app_glue.h>

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Config.h"
#include "Director.h"
#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"
#include "Input/Pointer.h"
#include "Platform/Android/ShakeGestureDetector.h"

using rainbow::Bundle;
using rainbow::Chrono;
using rainbow::Config;
using rainbow::Pointer;
using rainbow::ShakeGestureDetector;
using rainbow::Vec2i;
using rainbow::graphics::Context;

ANativeActivity* g_native_activity;

struct RainbowContext
{
    /// <summary>Whether the window is in focus.</summary>
    bool active = false;

    /// <summary>Whether the user has requested to quit.</summary>
    bool done = false;

    ASensorEventQueue* sensor_event_queue = nullptr;

    EGLDisplay display = EGL_NO_DISPLAY;
    EGLSurface surface = EGL_NO_SURFACE;
    EGLContext context = EGL_NO_CONTEXT;

    std::optional<Director> director;
};

void android_handle_display(RainbowContext*);
void android_handle_event(android_app*, int32_t cmd);
auto android_handle_input(android_app*, AInputEvent*) -> int32_t;
auto android_handle_motion(android_app*, AInputEvent*) -> int32_t;
void android_init_display(RainbowContext*, ANativeWindow*);
auto attach_rendering_context(RainbowContext*) -> EGLBoolean;
auto detach_rendering_context(RainbowContext*) -> EGLBoolean;
auto get_accelerometer(ASensorManager* manager = ASensorManager_getInstance())
    -> const ASensor*;
auto get_pointer_event(const Context&, AInputEvent*, int32_t index) -> Pointer;

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
}  // namespace

void android_main(android_app* app)
{
    pthread_key_create(&s_thread_key, detach_current_thread);

    JNIEnv* env;
    app->activity->vm->AttachCurrentThread(&env, nullptr);
    pthread_setspecific(s_thread_key, app->activity->vm);

    RainbowContext context;
    app->userData = &context;
    app->onAppCmd = android_handle_event;
    app->onInputEvent = android_handle_input;
    g_native_activity = app->activity;

    const Bundle bundle;
    rainbow::filesystem::initialize(bundle, nullptr, false);

#ifndef USE_HEIMDALL
    const Config config;
    if (config.needs_accelerometer())
#endif
    {
        auto sensor_manager = ASensorManager_getInstance();
        if (get_accelerometer(sensor_manager) != nullptr)
        {
            context.sensor_event_queue = ASensorManager_createEventQueue(  //
                sensor_manager,
                app->looper,
                LOOPER_ID_USER,
                nullptr,
                nullptr);
        }
    }

#ifdef USE_HEIMDALL
    ShakeGestureDetector shake_gesture_detector(
        [&context] { context.director->show_diagnostic_tools(); });
#endif

    Chrono chrono;
    while (!context.done)
    {
        int events;
        struct android_poll_source* source;
        while (ALooper_pollAll(!context.active ? -1 : 0,
                               nullptr,
                               &events,
                               reinterpret_cast<void**>(&source)) >= 0)
        {
            if (source != nullptr)
                source->process(app, source);

            if (!context.active || context.done)
                break;

            if (auto queue = context.sensor_event_queue)
            {
                ASensorEvent event;
                while (ASensorEventQueue_getEvents(queue, &event, 1) > 0)
                {
#ifdef USE_HEIMDALL
                    shake_gesture_detector.update(  //
                        {
                            event.acceleration.x,
                            event.acceleration.y,
                            event.acceleration.z,
                        },
                        event.timestamp);
#endif
                    context.director->input().accelerated(  //
                        event.acceleration.x,
                        event.acceleration.y,
                        event.acceleration.z,
                        event.timestamp);
                }
            }
        }

        chrono.tick();
        if (!context.active || !context.director)
            continue;

        context.director->update(chrono.delta());
        context.director->draw();
        eglSwapBuffers(context.display, context.surface);
    }

    context.director.reset();
    if (context.display != EGL_NO_DISPLAY)
    {
        detach_rendering_context(&context);
        if (context.context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(context.display, context.context);
            context.context = EGL_NO_CONTEXT;
        }
        if (context.surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(context.display, context.surface);
            context.surface = EGL_NO_SURFACE;
        }
        eglTerminate(context.display);
        context.display = EGL_NO_DISPLAY;
    }
}

void android_handle_display(RainbowContext* ctx)
{
    if (ctx->director)
        return;

    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(ctx->display, ctx->surface, EGL_WIDTH, &width);
    eglQuerySurface(ctx->display, ctx->surface, EGL_HEIGHT, &height);

    ctx->director.emplace();
    if (ctx->director->terminated() ||
        (ctx->director->init({width, height}), ctx->director->terminated()))
    {
        LOGF("%s", ctx->director->error().message().c_str());
        ctx->done = true;
    }
}

void android_init_display(RainbowContext* ctx, ANativeWindow* window)
{
    EGLDisplay& dpy = ctx->display;
    if (dpy == EGL_NO_DISPLAY)
    {
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(dpy, 0, 0);
    }

    constexpr EGLint attrib_list[]{
        EGL_ALPHA_SIZE,
        8,
        EGL_BLUE_SIZE,
        8,
        EGL_GREEN_SIZE,
        8,
        EGL_RED_SIZE,
        8,
        EGL_SURFACE_TYPE,
        EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_NONE,
    };
    EGLConfig config;
    EGLint nconfigs;
    eglChooseConfig(dpy, attrib_list, &config, 1, &nconfigs);

    EGLSurface& surface = ctx->surface;
    if (surface == EGL_NO_SURFACE)
    {
        EGLint format;
        eglGetConfigAttrib(dpy, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(window, 0, 0, format);

        surface = eglCreateWindowSurface(dpy, config, window, nullptr);
        if (surface == EGL_NO_SURFACE)
        {
            LOGF("Failed to create EGL window surface");
            return;
        }
    }

    EGLContext& context = ctx->context;
    if (context == EGL_NO_CONTEXT)
    {
        constexpr EGLint gles_attrib[]{EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
        context = eglCreateContext(dpy, config, EGL_NO_CONTEXT, gles_attrib);
        if (context == EGL_NO_CONTEXT)
        {
            LOGF("Failed to create EGL rendering context");
            return;
        }
    }

    ctx->done = attach_rendering_context(ctx) == EGL_FALSE;
    if (ctx->done)
        LOGF("Failed to bind context to the current rendering thread");
    else
        android_handle_display(ctx);
}

// https://developer.nvidia.com/fixing-common-android-lifecycle-issues-games
void android_handle_event(android_app* app, int32_t cmd)
{
    RainbowContext* ctx = static_cast<RainbowContext*>(app->userData);
    switch (cmd)
    {
        case APP_CMD_INIT_WINDOW:
            android_init_display(ctx, app->window);
            break;

        case APP_CMD_TERM_WINDOW:
            detach_rendering_context(ctx);
            eglDestroySurface(ctx->display, ctx->surface);
            ctx->surface = EGL_NO_SURFACE;
            break;

        case APP_CMD_WINDOW_RESIZED:
            android_handle_display(ctx);
            break;

        case APP_CMD_GAINED_FOCUS:
            if (auto queue = ctx->sensor_event_queue)
            {
                // 60 events per second (in usec).
                constexpr int32_t rate = (1000 * 1000) / 60;
                auto accelerometer_sensor = get_accelerometer();
                ASensorEventQueue_enableSensor(queue, accelerometer_sensor);
                ASensorEventQueue_setEventRate(
                    queue, accelerometer_sensor, rate);
            }
            ctx->director->on_focus_gained();
            ctx->active = true;
            break;

        case APP_CMD_LOST_FOCUS:
            break;

        case APP_CMD_LOW_MEMORY:
            ctx->director->on_memory_warning();
            break;

        case APP_CMD_RESUME:
            if (ctx->surface != EGL_NO_SURFACE)
            {
                attach_rendering_context(ctx);

                // From NVIDIA:
                // Note that in some cases, only the onPause is received on
                // suspend, in which case, the focus lost callback actually
                // comes when the device is resumed to the lock screen. In other
                // words, resuming sends an indication that you have been
                // resumed (onResume) and then an indication that you are hidden
                // (focus lost). And the unlock case remains unchanged (focus
                // regained).
                if (!ctx->active)
                    android_handle_event(app, APP_CMD_GAINED_FOCUS);
            }
            break;

        case APP_CMD_SAVE_STATE:
            break;

        case APP_CMD_PAUSE:
            detach_rendering_context(ctx);
            if (auto queue = ctx->sensor_event_queue)
                ASensorEventQueue_disableSensor(queue, get_accelerometer());
            ctx->director->on_focus_lost();
            ctx->active = false;
            break;

        case APP_CMD_DESTROY:
            ctx->active = false;
            ctx->done = true;
            break;

        default:
            break;
    }
}

auto android_handle_input(android_app* app, AInputEvent* event) -> int32_t
{
    switch (AInputEvent_getType(event))
    {
        case AINPUT_EVENT_TYPE_KEY:
            switch (AKeyEvent_getKeyCode(event))
            {
                case AKEYCODE_BACK:
                    return 1;
#ifdef USE_HEIMDALL
                case AKEYCODE_MENU:
                    if (AKeyEvent_getAction(event) == AKEY_EVENT_ACTION_DOWN)
                    {
                        static_cast<RainbowContext*>(app->userData)
                            ->director->show_diagnostic_tools();
                    }
                    return 1;
#endif
                default:
                    return 0;
            }

        case AINPUT_EVENT_TYPE_MOTION:
            return android_handle_motion(app, event);

        default:
            return 0;
    }
}

auto android_handle_motion(android_app* app, AInputEvent* event) -> int32_t
{
    auto& director = static_cast<RainbowContext*>(app->userData)->director;
    switch (AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK)
    {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
        {
            const int32_t index = (AMotionEvent_getAction(event) &
                                   AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            Pointer p =
                get_pointer_event(director->graphics_context(), event, index);
            director->input().on_pointer_began(ArrayView<Pointer>{p});
            break;
        }

        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
        {
            const int32_t index = (AMotionEvent_getAction(event) &
                                   AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                                  AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            Pointer p =
                get_pointer_event(director->graphics_context(), event, index);
            director->input().on_pointer_ended(ArrayView<Pointer>{p});
            break;
        }

        case AMOTION_EVENT_ACTION_MOVE:
        {
            const size_t count = AMotionEvent_getPointerCount(event);
            auto pointers = std::make_unique<Pointer[]>(count);
            std::generate_n(
                pointers.get(),
                count,
                [&ctx = director->graphics_context(), event, i = 0]() mutable {
                    return get_pointer_event(ctx, event, i++);
                });
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

auto attach_rendering_context(RainbowContext* ctx) -> EGLBoolean
{
    return eglMakeCurrent(
        ctx->display, ctx->surface, ctx->surface, ctx->context);
}

auto detach_rendering_context(RainbowContext* ctx) -> EGLBoolean
{
    return eglMakeCurrent(
        ctx->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

auto get_accelerometer(ASensorManager* manager) -> const ASensor*
{
    return ASensorManager_getDefaultSensor(manager, ASENSOR_TYPE_ACCELEROMETER);
}

auto get_pointer_event(const Context& ctx, AInputEvent* event, int32_t index)
    -> Pointer
{
    const Vec2i p(
        AMotionEvent_getX(event, index), AMotionEvent_getY(event, index));
    const auto point = rainbow::graphics::convert_to_flipped_view(ctx, p);
    return {
        static_cast<uint32_t>(AMotionEvent_getPointerId(event, index)),
        point.x,
        point.y,
        static_cast<uint64_t>(AMotionEvent_getEventTime(event)),
    };
}
