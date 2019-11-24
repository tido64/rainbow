// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/RainbowController.h"

#include <SDL.h>

#include "Config.h"
#include "Input/Controller.h"
#include "Input/Pointer.h"
#include "Platform/SDL/Context.h"

using rainbow::Config;
using rainbow::ControllerID;
using rainbow::RainbowController;
using rainbow::SDLContext;
using rainbow::Vec2i;

namespace
{
    constexpr unsigned int kInactiveSleepTime = 100;

    constexpr uint32_t kMouseButtons[]{
        SDL_BUTTON_LEFT,
        SDL_BUTTON_MIDDLE,
        SDL_BUTTON_RIGHT,
        SDL_BUTTON_X1,
        SDL_BUTTON_X2};

    auto is_fullscreen(const SDL_Keysym& keysym) -> bool
    {
#ifdef RAINBOW_OS_MACOS
        const auto mod = KMOD_LCTRL | KMOD_LGUI;
        return keysym.sym == SDLK_f && (keysym.mod & mod) == mod;
#else
        return keysym.sym == SDLK_RETURN &&
               (keysym.mod & KMOD_LALT) == KMOD_LALT;
#endif
    }

    auto is_quit([[maybe_unused]] const SDL_Keysym& keysym) -> bool
    {
#ifdef RAINBOW_OS_MACOS
        return false;
#else
        return keysym.sym == SDLK_q && (keysym.mod & KMOD_LCTRL);
#endif
    }
}  // namespace

RainbowController::RainbowController(SDLContext& context, const Config& config)
    : context_(context), suspend_on_focus_lost_(config.suspend())
{
    if (director_.terminated())
        return;

    for (int i = 0; i < SDL_NumJoysticks(); ++i)
        on_controller_connected(i);

    director_.init(context_.drawable_size());
    on_window_resized();

    chrono_.tick();
    LOGI("Initialization time: %" PRId64 " ms", chrono_.delta());
}

auto RainbowController::run() -> bool
{
    if (director_.terminated())
        return false;

    SDL_Event event;  // NOLINT(cppcoreguidelines-pro-type-member-init)
    while (SDL_PollEvent(&event) != 0)
    {
        switch (event.type)
        {
            case SDL_QUIT:
                director_.terminate();
                return false;
            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        on_window_resized();
                        director_.on_focus_gained();
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        if (suspend_on_focus_lost_)
                            director_.on_focus_gained();
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        if (suspend_on_focus_lost_)
                            director_.on_focus_lost();
                        break;
                    case SDL_WINDOWEVENT_CLOSE:
                        director_.terminate();
                        return false;
                    default:
                        break;
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.repeat == 0)
                {
                    const SDL_Keysym& keysym = event.key.keysym;
                    if (is_quit(keysym))
                    {
                        director_.terminate();
                        return false;
                    }
                    if (is_fullscreen(keysym))
                    {
                        // Unfocus Director while we resize the window to avoid
                        // glitches. Focus is restored when we receive an
                        // SDL_WINDOWEVENT_SIZE_CHANGED.
                        director_.on_focus_lost();
                        context_.toggle_fullscreen();
                    }
                    else
                    {
                        director_.input().on_key_down(
                            KeyStroke::from_event(keysym));
                    }
                }
                break;
            case SDL_KEYUP:
                director_.input().on_key_up(
                    KeyStroke::from_event(event.key.keysym));
                break;
            case SDL_MOUSEMOTION:
                on_mouse_motion(event.motion.state,
                                graphics::convert_to_flipped_view(
                                    director_.graphics_context(),
                                    Vec2i(event.motion.x, event.motion.y)),
                                event.motion.timestamp);
                break;
            case SDL_MOUSEBUTTONDOWN:
                on_mouse_down(event.button.button,
                              graphics::convert_to_flipped_view(
                                  director_.graphics_context(),
                                  Vec2i(event.button.x, event.button.y)),
                              event.button.timestamp);
                break;
            case SDL_MOUSEBUTTONUP:
                on_mouse_up(event.button.button,
                            graphics::convert_to_flipped_view(
                                director_.graphics_context(),
                                Vec2i(event.button.x, event.button.y)),
                            event.button.timestamp);
                break;
            case SDL_MOUSEWHEEL:
                director_.input().on_mouse_wheel(Pointer{
                    event.wheel.which,
#if SDL_VERSION_ATLEAST(2, 0, 4)
                    event.wheel.direction == SDL_MOUSEWHEEL_NORMAL
                        ? event.wheel.x
                        : event.wheel.x * -1,
#else
                    event.wheel.x,
#endif
                    event.wheel.y,
                    event.wheel.timestamp,
                });
                break;
            case SDL_CONTROLLERAXISMOTION:
                director_.input().on_controller_axis_motion(
                    ControllerAxisMotion{
                        event.caxis.which,
                        static_cast<ControllerAxis>(event.caxis.axis),
                        event.caxis.value,
                        event.caxis.timestamp});
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                director_.input().on_controller_button_down(
                    ControllerButtonEvent{
                        event.cbutton.which,
                        static_cast<ControllerButton>(event.cbutton.button),
                        event.cbutton.timestamp});
                break;
            case SDL_CONTROLLERBUTTONUP:
                director_.input().on_controller_button_up(ControllerButtonEvent(
                    event.cbutton.which,
                    static_cast<ControllerButton>(event.cbutton.button),
                    event.cbutton.timestamp));
                break;
            case SDL_CONTROLLERDEVICEADDED:
                on_controller_connected(event.cdevice.which);
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                on_controller_disconnected(event.cdevice.which);
                break;
            default:
                break;
        }
    }

    chrono_.tick();
    if (!director_.active())
        Chrono::sleep(kInactiveSleepTime);
    else
    {
        // Update game logic.
        director_.update(chrono_.delta());

        // Draw.
        director_.draw();
        context_.swap();
    }

    return true;
}

void RainbowController::on_controller_connected(ControllerID device_index)
{
    if (SDL_IsGameController(device_index) == 0)
        return;

    auto controller = SDL_GameControllerOpen(device_index);
    auto id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
    game_controllers_.emplace_back(id, controller);
    director_.input().on_controller_connected(id);
}

void RainbowController::on_controller_disconnected(ControllerID instance_id)
{
    for (auto i = game_controllers_.begin(); i < game_controllers_.end(); ++i)
    {
        if (std::get<0>(*i) == instance_id)
        {
            auto controller = std::get<1>(*i);
            if (SDL_GameControllerGetAttached(controller) != 0)
                SDL_GameControllerClose(controller);

            quick_erase(game_controllers_, i);
            director_.input().on_controller_disconnected(instance_id);
            break;
        }
    }
}

void RainbowController::on_mouse_down(uint32_t button,
                                      const Vec2i& point,
                                      uint64_t timestamp)
{
    Pointer p{button, point.x, point.y, timestamp};
    director_.input().on_pointer_began(p);
}

void RainbowController::on_mouse_motion(uint32_t buttons,
                                        const Vec2i& point,
                                        uint64_t timestamp)
{
    if (buttons > 0)
    {
        Pointer p[array_size(kMouseButtons)];
        size_t i = 0;
        for (auto button : kMouseButtons)
        {
            if ((buttons & SDL_BUTTON(button)) != 0)
            {
                p[i] = Pointer{button, point.x, point.y, timestamp};
                ++i;
            }
        }
        director_.input().on_pointer_moved({p, i});
    }
    else
    {
        Pointer p{0, point.x, point.y, timestamp};
        director_.input().on_pointer_moved(p);
    }
}

void RainbowController::on_mouse_up(uint32_t button,
                                    const Vec2i& point,
                                    uint64_t timestamp)
{
    Pointer p{button, point.x, point.y, timestamp};
    director_.input().on_pointer_ended(p);
}

void RainbowController::on_window_resized()
{
    auto& gfx = director_.graphics_context();
    const Vec2i& size = context_.window_size();
    if (size == gfx.window_size)
        return;

    const Vec2i& viewport = context_.drawable_size();
    graphics::set_window_size(gfx, size, viewport.x / size.x);
}
