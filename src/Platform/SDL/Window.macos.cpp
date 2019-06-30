// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

using rainbow::Config;
using rainbow::sdl::Window;

void Window::setup(const Config&)
{
    // Prevent the full screen window from being minimized when losing focus.
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
}
