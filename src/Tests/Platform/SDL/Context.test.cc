// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SDL/Context.h"

#include <gtest/gtest.h>

#include "Config.h"
#include "Tests/TestHelpers.h"

using rainbow::Config;
using rainbow::SDLContext;
using rainbow::test::ScopedAssetsDirectory;

auto is_hidpi()
{
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(0, 0, &mode);

    SDL_Rect bounds;
    SDL_GetDisplayBounds(0, &bounds);

    return mode.h > 1200 && mode.h > bounds.h;
}

TEST(SDLContextTest, CreatesLoDPIWindow)
{
    ScopedAssetsDirectory scoped_assets("ConfigTest_NoConfiguration");
    if (SDLContext context{{}})
    {
        auto drawable_size = context.drawable_size();
        ASSERT_EQ(drawable_size.x, 1280);
        ASSERT_EQ(drawable_size.y, 720);

        auto window_size = context.window_size();
        ASSERT_EQ(window_size.x, 1280);
        ASSERT_EQ(window_size.y, 720);
    }
}

TEST(SDLContextTest, CreatesHiDPIWindow)
{
    ScopedAssetsDirectory scoped_assets{"ConfigTest_SparseConfiguration"};
    if (SDLContext context{{}})
    {
        auto drawable_size = context.drawable_size();
        if (is_hidpi())
        {
            ASSERT_EQ(drawable_size.x, 2560);
            ASSERT_EQ(drawable_size.y, 1440);
        }
        else
        {
            ASSERT_EQ(drawable_size.x, 1280);
            ASSERT_EQ(drawable_size.y, 720);
        }

        auto window_size = context.window_size();
        ASSERT_EQ(window_size.x, 1280);
        ASSERT_EQ(window_size.y, 720);
    }
}
