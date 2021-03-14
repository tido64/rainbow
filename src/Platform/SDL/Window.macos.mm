// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Window.h"

#include <AppKit/AppKit.h>
#include <SDL_syswm.h>

#include "Common/Logging.h"

namespace
{
    auto content_view(SDL_Window* window)
    {
        SDL_SysWMinfo wm_info;
        SDL_VERSION(&wm_info.version);
        if (!SDL_GetWindowWMInfo(window, &wm_info)) {
            R_ABORT("SDL: Failed to get window information: %s",  //
                    SDL_GetError());
        }
        return wm_info.info.cocoa.window.contentView;
    }
}

void rainbow::set_hidpi_mode(SDL_Window* window, bool hidpi)
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    content_view(window).wantsBestResolutionOpenGLSurface = hidpi;
#pragma clang diagnostic pop
}
