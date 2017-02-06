// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SDL_UTILITIES_H_
#define PLATFORM_SDL_UTILITIES_H_

struct SDL_Window;

namespace rainbow
{
    /// <summary>Sets window icon.</summary>
    /// <param name="window">Target window.</param>
    /// <param name="icon_id">Icon resource id.</param>
    /// <param name="small_size">Size of small icon.</param>
    /// <param name="big_size">Size of big icon.</param>
    void set_window_icon(SDL_Window* window,
                         int icon_id,
                         int small_size = 32,
                         int big_size = 256);

    /// <summary>Sets window icon from embedded resources.</summary>
    /// <param name="window">Target window.</param>
    inline void set_window_icon(SDL_Window* window)
    {
        constexpr int IDI_ICON1 = 101;
        set_window_icon(window, IDI_ICON1);
    }
}

// | Application icon sizes                                                    |
// |---------------------------------------------------------------------------|
// | *nix          | 16×16, 24×24, 48×48, 96×96                                |
// | Android       | 24×24, 48×48, 192×192                                     |
// | iOS 9         | 58×58, 80×80, 120×120, 152×152, 180×180, 1024×1024        |
// | macOS         | 16×16, 32×32, 64×64, 128×128, 256×256, 512×512, 1024×1024 |
// | Windows 10    | 16×16, 24×24, 32×32, 48×48, 256×256                       |
// | Windows Phone | 62×62, 99×99, 173×173, 200×200                            |

#endif
