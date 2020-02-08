// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_WINDOW_H_
#define PLATFORM_SDL_WINDOW_H_

struct SDL_Window;

namespace rainbow
{
    void set_hidpi_mode(SDL_Window*, bool hidpi);
}

#endif
