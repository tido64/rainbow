// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_RAINBOWCONTROLLER_H_
#define PLATFORM_SDL_RAINBOWCONTROLLER_H_

#include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#include <SDL_gamecontroller.h>

#include "Director.h"

namespace rainbow
{
    using GameController = std::tuple<int, SDL_GameController*>;

    class Config;
    class SDLContext;

    class RainbowController
    {
    public:
        RainbowController(SDLContext& context, const Config& config);

        auto error() const -> std::error_code { return director_.error(); }

        bool run();

    private:
        SDLContext& context_;
        Chrono chrono_;
        ::Director director_;
        const bool suspend_on_focus_lost_;
        std::vector<GameController> game_controllers_;

        void on_controller_connected(ControllerID device_index);
        void on_controller_disconnected(ControllerID instance_id);

        void on_mouse_down(uint32_t button,
                           const Vec2i& point,
                           uint64_t timestamp);

        void on_mouse_motion(uint32_t buttons,
                             const Vec2i& point,
                             uint64_t timestamp);

        void on_mouse_up(uint32_t button,
                         const Vec2i& point,
                         uint64_t timestamp);

        void on_window_resized();
    };
}  // namespace rainbow

#endif
