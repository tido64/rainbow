// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Graphics/TextureAtlas.h"
#include "Tests/TestHelpers.h"

using rainbow::TextureAtlas;
using rainbow::Vec2f;

TEST(TextureAtlasTest, AddsRegions)
{
    TextureAtlas atlas{rainbow::ISolemnlySwearThatIAmOnlyTesting{}};

    ASSERT_EQ(atlas.size(), 0u);

    const unsigned int regions[]{
        atlas.add_region(1, 1, 1, 1),
        atlas.add_region(2, 2, 2, 2),
        atlas.add_region(3, 3, 3, 3),
    };

    ASSERT_EQ(atlas.size(), 3u);

    int check = 0;
    for (auto i : regions)
    {
        const float value = ++check / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(region.vx[3], Vec2f(value, value));
        ASSERT_EQ(region.vx[2], Vec2f(value + value, value));
        ASSERT_EQ(region.vx[1], Vec2f(value + value, value + value));
        ASSERT_EQ(region.vx[0], Vec2f(value, value + value));
    }
}

TEST(TextureAtlasTest, SetsRegions)
{
    const int regions[]{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
    TextureAtlas atlas{rainbow::ISolemnlySwearThatIAmOnlyTesting{}};
    atlas.set_regions(regions);

    ASSERT_EQ(atlas.size(), 3u);

    atlas.add_region(4, 4, 4, 4);

    ASSERT_EQ(atlas.size(), 4u);

    for (unsigned int i = 0; i < atlas.size(); ++i)
    {
        const float value = (i + 1) / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(region.vx[3], Vec2f(value, value));
        ASSERT_EQ(region.vx[2], Vec2f(value + value, value));
        ASSERT_EQ(region.vx[1], Vec2f(value + value, value + value));
        ASSERT_EQ(region.vx[0], Vec2f(value, value + value));
    }

    const int regions2[]{4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6};
    atlas.set_regions(regions2);

    ASSERT_EQ(atlas.size(), 3u);

    for (unsigned int i = 0; i < atlas.size(); ++i)
    {
        const float value = (i + 4) / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(region.vx[3], Vec2f(value, value));
        ASSERT_EQ(region.vx[2], Vec2f(value + value, value));
        ASSERT_EQ(region.vx[1], Vec2f(value + value, value + value));
        ASSERT_EQ(region.vx[0], Vec2f(value, value + value));
    }
}
