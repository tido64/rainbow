// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Utilities.h"

#include "Platform/Macros.h"

#if defined(RAINBOW_OS_WINDOWS)
#include <SDL_syswm.h>

namespace
{
    class EmbeddedIcon
    {
    public:
        EmbeddedIcon(int resource_id, int size)
            : icon_(static_cast<HICON>(LoadImage(
                  GetModuleHandle(nullptr),
                  MAKEINTRESOURCE(resource_id),
                  IMAGE_ICON,
                  size,
                  size,
                  (size == 0 ? LR_DEFAULTSIZE : LR_DEFAULTCOLOR) | LR_SHARED)))
        {
        }

        ~EmbeddedIcon() { DestroyIcon(icon_); }

        explicit operator LPARAM() const
        {
            return reinterpret_cast<LPARAM>(icon_);
        }

    private:
        HICON icon_;
    };
}

void rainbow::set_window_icon(SDL_Window* window,
                              int icon_id,
                              int small_size,
                              int big_size)
{
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!SDL_GetWindowWMInfo(window, &info))
        return;

    SendMessage(info.info.win.window,
                WM_SETICON,
                ICON_SMALL,
                static_cast<LPARAM>(EmbeddedIcon{icon_id, small_size}));
    SendMessage(info.info.win.window,
                WM_SETICON,
                ICON_BIG,
                static_cast<LPARAM>(EmbeddedIcon{icon_id, big_size}));
}

#else

void rainbow::set_window_icon(SDL_Window*, int, int, int)
{
}

#endif
