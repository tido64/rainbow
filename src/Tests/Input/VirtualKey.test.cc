// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/VirtualKey.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_SDL
#   include <SDL_config.h>
#   include <SDL_keyboard.h>
#endif  // RAINBOW_SDL

#include <gtest/gtest.h>

using rainbow::VirtualKey;

TEST(VirtualKeyTest, ScancodeToVirtualKeyMappingIsConsistent)
{
    for (auto i = static_cast<int>(VirtualKey::Unknown);
         i < static_cast<int>(VirtualKey::KeyCount);
         ++i)
    {
        const auto vk = static_cast<VirtualKey>(i);
#ifdef RAINBOW_SDL
        SDL_Keysym keysym{};
        keysym.scancode = static_cast<SDL_Scancode>(rainbow::to_scancode(vk));
#endif  // RAINBOW_SDL
        ASSERT_EQ(rainbow::to_virtualkey(keysym), vk);
    }
}
