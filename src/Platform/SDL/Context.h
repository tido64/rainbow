// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_CONTEXT_H_
#define PLATFORM_SDL_CONTEXT_H_

#include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#include <SDL.h>

#include "Math/Vec2.h"

namespace rainbow
{
    class Config;

    class SDLContext
    {
    public:
        SDLContext(const Config& config);
        ~SDLContext();

        auto drawable_size() const -> Vec2i
        {
            Vec2i size;
            SDL_GL_GetDrawableSize(window_, &size.x, &size.y);
            return size;
        }

        auto window_size() const -> Vec2i
        {
            Vec2i size;
            SDL_GetWindowSize(window_, &size.x, &size.y);
            return size;
        }

        void swap() const;
        void toggle_fullscreen();

        explicit operator bool() const { return context_; }

    private:
        SDL_Window* window_;     ///< Window handle.
        bool vsync_;             ///< Whether vertical sync is enabled.
        uint32_t fullscreen_;    ///< Whether the window is in full screen mode.
        SDL_GLContext context_;  ///< OpenGL context handle.
    };
}  // namespace rainbow

#endif
