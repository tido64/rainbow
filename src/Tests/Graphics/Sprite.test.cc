// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <functional>

#include <gtest/gtest.h>

#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Tests/TestHelpers.h"

using rainbow::Color;
using rainbow::SharedPtr;
using rainbow::Sprite;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::TextureAtlas;
using rainbow::Vec2f;

namespace
{
    constexpr unsigned int kStaleBuffer     = 1u << 0;
    constexpr unsigned int kStalePosition   = 1u << 1;
    constexpr unsigned int kStaleTexture    = 1u << 2;
    //constexpr unsigned int kStaleNormalMap  = 1u << 3;
    constexpr unsigned int kStaleMask       = 0xffffu;

    rainbow::ISolemnlySwearThatIAmOnlyTesting mock;

    auto create_texture()
    {
        auto atlas = rainbow::make_shared<TextureAtlas>(mock);
        atlas->add_region(0, 0, 1, 1);
        return atlas;
    }

    auto is_stale(const Sprite& sprite)
    {
        return (sprite.state() & kStaleMask) != 0;
    }

    auto is_stale(const Sprite& sprite, unsigned int state)
    {
        return (sprite.state() & state) == state;
    }

    class SpriteFlipMirrorTest : public ::testing::Test
    {
    public:
        SpriteFlipMirrorTest() : atlas(create_texture()) {}

    protected:
        Sprite sprite = {2, 2};
        SpriteVertex vertex_array[4];
        SharedPtr<TextureAtlas> atlas;
        unsigned int texture = 0;
        Vec2f p[4];
        Vec2f u[4];

        void update() { sprite.update(vertex_array, *atlas); }

        void SetUp() override
        {
            sprite.set_texture(texture);
            update();
            for (size_t i = 0; i < 4; ++i)
            {
                p[i] = vertex_array[i].position;
                u[i] = vertex_array[i].texcoord;
            }
        }
    };

    class SpriteHiddenTest : public ::testing::Test
    {
    public:
        SpriteHiddenTest() : atlas(create_texture()) {}

    protected:
        Sprite sprite = {2, 2};
        SpriteVertex vertex_array[4];
        SharedPtr<TextureAtlas> atlas;

        void update() { sprite.update(vertex_array, *atlas); }

        void SetUp() override
        {
            update();
        }
    };
}

TEST(SpriteTest, PlacedAtOriginOnCreation)
{
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_TRUE(sprite.position().is_zero());

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, *create_texture());

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));
}

TEST(SpriteTest, CanBeColored)
{
    constexpr Color color(0xec, 0xef, 0xf1);
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_NE(sprite.color(), color);

    sprite.set_color(color);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer | kStaleTexture));
    ASSERT_EQ(sprite.color(), color);

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, *create_texture());

    ASSERT_FALSE(is_stale(sprite));
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].color, color);
}

TEST(SpriteTest, CanBeTagged)
{
    Sprite sprite(2, 2);

    ASSERT_EQ(sprite.id(), Sprite::kNoId);

    const int tag = 0xdeadbeef;
    sprite.set_id(tag);

    ASSERT_EQ(sprite.id(), tag);
}

TEST(SpriteTest, ChangesPivotWithoutShifting)
{
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.pivot(), Vec2f(0.5f, 0.5f));

    SpriteVertex vertex_array[4];
    auto texture = create_texture();
    sprite.update(vertex_array, *texture);

    ASSERT_FALSE(is_stale(sprite));

    sprite.set_pivot(Vec2f::Zero);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(sprite.pivot(), Vec2f::Zero);
    ASSERT_EQ(sprite.position(), -Vec2f::One);
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));

    sprite.set_position(Vec2f::Zero);

    ASSERT_TRUE(is_stale(sprite, kStalePosition));
    ASSERT_TRUE(sprite.position().is_zero());

    sprite.update(vertex_array, *texture);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(0, 0));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, 0));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(0, 2));
}

TEST(SpriteTest, Translates)
{
    Sprite sprite(2, 2);
    sprite.set_position(Vec2f::One);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, *create_texture());

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(sprite.position(), Vec2f::One);
    ASSERT_EQ(vertex_array[0].position, Vec2f(0, 0));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, 0));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(0, 2));

    sprite.move(-Vec2f::One);

    ASSERT_TRUE(is_stale(sprite, kStalePosition));
    ASSERT_TRUE(sprite.position().is_zero());
}

TEST(SpriteTest, Rotates)
{
    auto not_equal = std::not_fn(rainbow::are_equal<float>);

    Sprite sprite(4, 2);

    ASSERT_EQ(sprite.angle(), 0.0f);

    SpriteVertex vertex_array[4];
    auto texture = create_texture();
    for (int i = 0; i < 2; ++i)
    {
        const float scale = 1.0f + 0.5f * i;
        const float width = sprite.width() * scale;
        const float height = sprite.height() * scale;
        sprite.set_scale(scale);
        for (int deg = 0; deg <= 360; ++deg)
        {
            const float rad = rainbow::radians<float>(deg);
            sprite.set_rotation(rad);

            ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
            ASSERT_EQ(sprite.angle(), rad);

            sprite.update(vertex_array, *texture);
            const Vec2f p[]{vertex_array[0].position,
                            vertex_array[1].position,
                            vertex_array[2].position,
                            vertex_array[3].position};

            ASSERT_FALSE(is_stale(sprite));
            if (deg == 0 || deg == 360)
                ASSERT_FLOAT_EQ(scale + scale, p[2].x);
            else
                ASSERT_PRED2(not_equal, p[2].x, scale + scale);
            ASSERT_EQ(p[0], -p[2]);
            ASSERT_EQ(p[1], -p[3]);
            ASSERT_FLOAT_EQ(width, p[0].distance(p[1]));
            ASSERT_FLOAT_EQ(height, p[1].distance(p[2]));
            ASSERT_FLOAT_EQ(width, p[2].distance(p[3]));
            ASSERT_FLOAT_EQ(height, p[3].distance(p[0]));
        }
    }

    sprite.set_rotation(0.0f);
    sprite.set_scale(1.0f);
    sprite.update(vertex_array, *texture);

    const float width = sprite.width();
    const float height = sprite.height();

    float angle = 0;
    int i = 1;
    for (int deg = 1; deg <= 4; ++deg)
    {
        const float r = rainbow::radians<float>(deg * i);
        angle += r;
        sprite.rotate(r);

        ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
        ASSERT_FLOAT_EQ(angle, sprite.angle());

        sprite.update(vertex_array, *texture);
        const Vec2f p[]{vertex_array[0].position,
                        vertex_array[1].position,
                        vertex_array[2].position,
                        vertex_array[3].position};

        ASSERT_FALSE(is_stale(sprite));
        ASSERT_EQ(p[0], -p[2]);
        ASSERT_EQ(p[1], -p[3]);
        ASSERT_FLOAT_EQ(width, p[0].distance(p[1]));
        ASSERT_FLOAT_EQ(height, p[1].distance(p[2]));
        ASSERT_FLOAT_EQ(width, p[2].distance(p[3]));
        ASSERT_FLOAT_EQ(height, p[3].distance(p[0]));

        i *= -1;
    }
}

TEST(SpriteTest, Scales)
{
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f::One);

    sprite.set_scale(2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f(2.0f, 2.0f));

    SpriteVertex vertex_array[4];
    auto texture = create_texture();
    sprite.update(vertex_array, *texture);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-2, -2));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, -2));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-2, 2));

    sprite.set_scale(1);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f::One);

    sprite.update(vertex_array, *texture);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));

    sprite.set_scale(Vec2f(1, 2));

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f(1.0f, 2.0f));

    sprite.update(vertex_array, *texture);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -2));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -2));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 2));
}

TEST_F(SpriteFlipMirrorTest, IsNotFlippedOrMirroredInitially)
{
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());
    ASSERT_FALSE(is_stale(sprite));

    update();

    ASSERT_FALSE(is_stale(sprite));
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(vertex_array[i].position, p[i]);
        ASSERT_EQ(vertex_array[i].texcoord, u[i]);
    }
}

TEST_F(SpriteFlipMirrorTest, FlipsVertically)
{
    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[3]);
    ASSERT_EQ(vertex_array[1].texcoord, u[2]);
    ASSERT_EQ(vertex_array[2].texcoord, u[1]);
    ASSERT_EQ(vertex_array[3].texcoord, u[0]);

    sprite.set_texture(texture);

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    ASSERT_EQ(vertex_array[0].texcoord, u[3]);
    ASSERT_EQ(vertex_array[1].texcoord, u[2]);
    ASSERT_EQ(vertex_array[2].texcoord, u[1]);
    ASSERT_EQ(vertex_array[3].texcoord, u[0]);

    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(vertex_array[i].position, p[i]);
        ASSERT_EQ(vertex_array[i].texcoord, u[i]);
    }
}

TEST_F(SpriteFlipMirrorTest, Mirrors)
{
    sprite.mirror();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_TRUE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[1]);
    ASSERT_EQ(vertex_array[1].texcoord, u[0]);
    ASSERT_EQ(vertex_array[2].texcoord, u[3]);
    ASSERT_EQ(vertex_array[3].texcoord, u[2]);

    sprite.set_texture(texture);
    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    ASSERT_EQ(vertex_array[0].texcoord, u[1]);
    ASSERT_EQ(vertex_array[1].texcoord, u[0]);
    ASSERT_EQ(vertex_array[2].texcoord, u[3]);
    ASSERT_EQ(vertex_array[3].texcoord, u[2]);

    sprite.mirror();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(vertex_array[i].position, p[i]);
        ASSERT_EQ(vertex_array[i].texcoord, u[i]);
    }
}

TEST_F(SpriteFlipMirrorTest, FlipsAndMirrors)
{
    sprite.flip();
    sprite.mirror();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_TRUE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[2]);
    ASSERT_EQ(vertex_array[1].texcoord, u[3]);
    ASSERT_EQ(vertex_array[2].texcoord, u[0]);
    ASSERT_EQ(vertex_array[3].texcoord, u[1]);

    sprite.set_texture(texture);
    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    ASSERT_EQ(vertex_array[0].texcoord, u[2]);
    ASSERT_EQ(vertex_array[1].texcoord, u[3]);
    ASSERT_EQ(vertex_array[2].texcoord, u[0]);
    ASSERT_EQ(vertex_array[3].texcoord, u[1]);

    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_TRUE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[1]);
    ASSERT_EQ(vertex_array[1].texcoord, u[0]);
    ASSERT_EQ(vertex_array[2].texcoord, u[3]);
    ASSERT_EQ(vertex_array[3].texcoord, u[2]);

    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_TRUE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[2]);
    ASSERT_EQ(vertex_array[1].texcoord, u[3]);
    ASSERT_EQ(vertex_array[2].texcoord, u[0]);
    ASSERT_EQ(vertex_array[3].texcoord, u[1]);

    sprite.mirror();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[3]);
    ASSERT_EQ(vertex_array[1].texcoord, u[2]);
    ASSERT_EQ(vertex_array[2].texcoord, u[1]);
    ASSERT_EQ(vertex_array[3].texcoord, u[0]);

    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(vertex_array[i].position, p[i]);
        ASSERT_EQ(vertex_array[i].texcoord, u[i]);
    }

    sprite.mirror();
    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_TRUE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[2]);
    ASSERT_EQ(vertex_array[1].texcoord, u[3]);
    ASSERT_EQ(vertex_array[2].texcoord, u[0]);
    ASSERT_EQ(vertex_array[3].texcoord, u[1]);

    sprite.mirror();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_TRUE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].position, p[i]);
    ASSERT_EQ(vertex_array[0].texcoord, u[3]);
    ASSERT_EQ(vertex_array[1].texcoord, u[2]);
    ASSERT_EQ(vertex_array[2].texcoord, u[1]);
    ASSERT_EQ(vertex_array[3].texcoord, u[0]);

    sprite.flip();

    ASSERT_TRUE(is_stale(sprite, kStaleTexture));
    ASSERT_FALSE(sprite.is_flipped());
    ASSERT_FALSE(sprite.is_mirrored());

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(vertex_array[i].position, p[i]);
        ASSERT_EQ(vertex_array[i].texcoord, u[i]);
    }
}

TEST_F(SpriteHiddenTest, IgnoresMultipleCalls)
{
    ASSERT_FALSE(sprite.is_hidden());

    sprite.hide();
    ASSERT_TRUE(sprite.is_hidden());
    sprite.hide();
    ASSERT_TRUE(sprite.is_hidden());

    sprite.show();
    ASSERT_FALSE(sprite.is_hidden());
    sprite.show();
    ASSERT_FALSE(sprite.is_hidden());
}

TEST_F(SpriteHiddenTest, RestoresHiddenSprites)
{
    sprite.hide();

    ASSERT_TRUE(is_stale(sprite, kStaleMask));

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    ASSERT_EQ(vertex_array[0].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[1].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[2].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[3].position, Vec2f::Zero);

    sprite.show();

    ASSERT_TRUE(is_stale(sprite, kStaleMask));

    update();

    ASSERT_FALSE(is_stale(sprite, kStaleMask));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));
}

TEST_F(SpriteHiddenTest, CanBeMoved)
{
    sprite.hide();
    sprite.set_position(Vec2f::One);
    update();

    ASSERT_EQ(vertex_array[0].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[1].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[2].position, Vec2f::Zero);
    ASSERT_EQ(vertex_array[3].position, Vec2f::Zero);

    sprite.show();

    ASSERT_TRUE(is_stale(sprite, kStaleMask));

    update();

    ASSERT_EQ(vertex_array[0].position, Vec2f(0, 0));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, 0));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(0, 2));
}

TEST(SpriteTest, UpdatesOnlyOnChange)
{
    Sprite sprite(2, 2);
    SpriteVertex vertex_array[4];
    auto texture = create_texture();

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_TRUE(sprite.update(vertex_array, *texture));
    ASSERT_FALSE(sprite.update(vertex_array, *texture));

    sprite.move(Vec2f::Left);

    ASSERT_TRUE(is_stale(sprite, kStalePosition));
    ASSERT_TRUE(sprite.update(vertex_array, *texture));
    ASSERT_FALSE(sprite.update(vertex_array, *texture));
}

TEST(SpriteTest, ManuallyConstructedRefsAreInvalid)
{
    ASSERT_FALSE(SpriteRef{});
}
