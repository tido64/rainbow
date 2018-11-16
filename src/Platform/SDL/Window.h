// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_WINDOW_H_
#define PLATFORM_SDL_WINDOW_H_

// Ensure we include the correct SDL_config.h by explicitly including it first.
// clang-format off
#include <SDL_config.h>
#include <SDL.h>
// clang-format on

#include "Common/Logging.h"

namespace rainbow
{
    class Config;
}

namespace rainbow::sdl
{
    template <typename T>
    class Context
    {
    public:
        Context()
        {
            if (SDL_Init(subsystems()) < 0)
                LOGF("SDL: Unable to initialise video: %s", SDL_GetError());
        }

        ~Context()
        {
            if (!is_initialized())
                return;

            SDL_Quit();
        }

        bool is_initialized() const { return SDL_WasInit(subsystems()) != 0; }

        auto subsystems() const -> uint32_t
        {
            return SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER;
        }
    };

    class Window : public Context<Window>
    {
    public:
        Window(const Config&);
        ~Window();

        operator SDL_Window*() { return window_; }

        explicit operator bool() const { return window_ != nullptr; }

    private:
        SDL_Window* window_ = nullptr;

        void setup(const Config&);
    };

    void open_window(const Config&);
}  // namespace rainbow::sdl

#endif
