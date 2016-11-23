// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
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

    ASSERT_EQ(0u, atlas.size());

    const unsigned int regions[]{
        atlas.add_region(1, 1, 1, 1),
        atlas.add_region(2, 2, 2, 2),
        atlas.add_region(3, 3, 3, 3),
    };

    ASSERT_EQ(3u, atlas.size());

    int check = 0;
    for (auto i : regions)
    {
        const float value = ++check / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(Vec2f(value, value), region.vx[3]);
        ASSERT_EQ(Vec2f(value + value, value), region.vx[2]);
        ASSERT_EQ(Vec2f(value + value, value + value), region.vx[1]);
        ASSERT_EQ(Vec2f(value, value + value), region.vx[0]);
    }
}

TEST(TextureAtlasTest, SetsRegions)
{
    const int regions[]{1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};
    TextureAtlas atlas{rainbow::ISolemnlySwearThatIAmOnlyTesting{}};
    atlas.set_regions(regions);

    ASSERT_EQ(3u, atlas.size());

    atlas.add_region(4, 4, 4, 4);

    ASSERT_EQ(4u, atlas.size());

    for (unsigned int i = 0; i < atlas.size(); ++i)
    {
        const float value = (i + 1) / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(Vec2f(value, value), region.vx[3]);
        ASSERT_EQ(Vec2f(value + value, value), region.vx[2]);
        ASSERT_EQ(Vec2f(value + value, value + value), region.vx[1]);
        ASSERT_EQ(Vec2f(value, value + value), region.vx[0]);
    }

    const int regions2[]{4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6};
    atlas.set_regions(regions2);

    ASSERT_EQ(3u, atlas.size());

    for (unsigned int i = 0; i < atlas.size(); ++i)
    {
        const float value = (i + 4) / 64.0f;
        auto& region = atlas[i];

        ASSERT_EQ(Vec2f(value, value), region.vx[3]);
        ASSERT_EQ(Vec2f(value + value, value), region.vx[2]);
        ASSERT_EQ(Vec2f(value + value, value + value), region.vx[1]);
        ASSERT_EQ(Vec2f(value, value + value), region.vx[0]);
    }
}
