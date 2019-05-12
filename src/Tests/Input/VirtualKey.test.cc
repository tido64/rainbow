// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/VirtualKey.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL
#    include <SDL_config.h>  // Ensure we include the correct SDL_config.h.
#    include <SDL.h>
#endif  // RAINBOW_SDL

#include <gtest/gtest.h>

#include "Common/TypeCast.h"

using namespace rainbow;

#ifdef RAINBOW_SDL
TEST(VirtualKeyTest, DISABLED_MapsKeyCode)
{
    // Only SDL_VideoInit() will call SDL_KeyboardInit(). If video
    // initialisation fails, we can't test to_keycode().
    if (SDL_VideoInit(nullptr) == 0)
    {
        constexpr auto begin = to_underlying_type(VirtualKey::Unknown) + 1;
        constexpr auto end = to_underlying_type(VirtualKey::KeyCount);
        for (auto i = begin; i < end; ++i)
        {
            const auto vk = static_cast<VirtualKey>(i);
            ASSERT_EQ(SDL_GetScancodeFromKey(to_keycode(vk)),
                      static_cast<SDL_Scancode>(to_scancode(vk)));
        }
    }

    SDL_Quit();
}

TEST(VirtualKeyTest, MapsKeyMods)
{
    constexpr std::tuple<int, uint32_t> data_set[]{
        {KMOD_NONE, KeyMods::None},
        {KMOD_LSHIFT, KeyMods::LeftShift},
        {KMOD_RSHIFT, KeyMods::RightShift},
        {KMOD_LCTRL, KeyMods::LeftCtrl},
        {KMOD_RCTRL, KeyMods::RightCtrl},
        {KMOD_LALT, KeyMods::LeftAlt},
        {KMOD_RALT, KeyMods::RightAlt},
        {KMOD_LGUI, KeyMods::LeftSuper},
        {KMOD_RGUI, KeyMods::RightSuper},
        {KMOD_NUM, KeyMods::None},   // Unimplemented
        {KMOD_CAPS, KeyMods::None},  // Unimplemented
        {KMOD_MODE, KeyMods::None},  // Unimplemented
        // Left + right combinations
        {KMOD_CTRL, KeyMods::Ctrl},
        {KMOD_SHIFT, KeyMods::Shift},
        {KMOD_ALT, KeyMods::Alt},
        {KMOD_GUI, KeyMods::Super},
        // Mixed combinations
        {KMOD_LSHIFT | KMOD_LCTRL, KeyMods::LeftShift | KeyMods::LeftCtrl},
        {KMOD_LSHIFT | KMOD_LALT, KeyMods::LeftShift | KeyMods::LeftAlt},
        {KMOD_LCTRL | KMOD_LALT, KeyMods::LeftCtrl | KeyMods::LeftAlt},
        {KMOD_LSHIFT | KMOD_LCTRL | KMOD_LALT,
         KeyMods::LeftShift | KeyMods::LeftCtrl | KeyMods::LeftAlt},
        {KMOD_LSHIFT | KMOD_LGUI, KeyMods::LeftShift | KeyMods::LeftSuper},
        {KMOD_LSHIFT | KMOD_LALT, KeyMods::LeftShift | KeyMods::LeftAlt},
        {KMOD_LGUI | KMOD_LALT, KeyMods::LeftSuper | KeyMods::LeftAlt},
        {KMOD_LSHIFT | KMOD_LGUI | KMOD_LALT,
         KeyMods::LeftShift | KeyMods::LeftSuper | KeyMods::LeftAlt},
    };

    for (auto [mod, expected] : data_set)
    {
        SDL_Keysym keysym{};
        keysym.mod = mod;
        ASSERT_EQ(KeyStroke::from_event(keysym).mods, expected);
    }
}

TEST(VirtualKeyTest, MapsScanCode)
{
    constexpr auto begin = to_underlying_type(VirtualKey::Unknown);
    constexpr auto end = to_underlying_type(VirtualKey::KeyCount);
    for (auto i = begin; i < end; ++i)
    {
        const auto vk = static_cast<VirtualKey>(i);
        SDL_Keysym keysym{};
        keysym.scancode = static_cast<SDL_Scancode>(to_scancode(vk));
        ASSERT_EQ(to_virtualkey(keysym), vk);
    }
}
#endif  // RAINBOW_SDL
