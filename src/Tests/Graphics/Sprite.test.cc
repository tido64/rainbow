// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Sprite.h"

#include <functional>

#include <gtest/gtest.h>

#include "Graphics/Texture.h"
#include "Tests/TestHelpers.h"

using rainbow::Color;
using rainbow::Sprite;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::Vec2f;
using rainbow::graphics::TextureData;

namespace
{
    constexpr unsigned int kStaleBuffer     = 1U << 0;
    constexpr unsigned int kStalePosition   = 1U << 1;
    constexpr unsigned int kStaleTexture    = 1U << 2;
    //constexpr unsigned int kStaleNormalMap  = 1u << 3;
    constexpr unsigned int kStaleMask       = 0xffffU;

    auto is_stale(const Sprite& sprite)
    {
        return (sprite.state() & kStaleMask) != 0;
    }

    auto is_stale(const Sprite& sprite, unsigned int state)
    {
        return (sprite.state() & state) == state;
    }

    auto mock_texture() { return TextureData{{}, 64, 64}; }

    class SpriteFlipMirrorTest : public ::testing::Test
    {
    protected:
        Sprite sprite{2, 2};
        SpriteVertex vertex_array[4];
        unsigned int texture = 0;
        std::array<Vec2f, 4> p;
        std::array<Vec2f, 4> u;

        void update() { sprite.update(vertex_array, mock_texture()); }

        void SetUp() override
        {
            sprite.texture({0, 0, 1, 1});
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
    protected:
        Sprite sprite{2, 2};
        SpriteVertex vertex_array[4];

        void update() { sprite.update(vertex_array, mock_texture()); }

        void SetUp() override { update(); }
    };
}  // namespace

TEST(SpriteTest, PlacedAtOriginOnCreation)
{
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_TRUE(sprite.position().is_zero());

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, mock_texture());

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

    sprite.color(color);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer | kStaleTexture));
    ASSERT_EQ(sprite.color(), color);

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, mock_texture());

    ASSERT_FALSE(is_stale(sprite));
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(vertex_array[i].color, color);
}

TEST(SpriteTest, CanBeTagged)
{
    Sprite sprite(2, 2);

    ASSERT_EQ(sprite.id(), Sprite::kNoId);

    const int tag = 0xdeadbeef;
    sprite.id(tag);

    ASSERT_EQ(sprite.id(), tag);
}

TEST(SpriteTest, ChangesPivotWithoutShifting)
{
    Sprite sprite(2, 2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.pivot(), Vec2f(0.5F, 0.5F));

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, mock_texture());

    ASSERT_FALSE(is_stale(sprite));

    sprite.pivot(Vec2f::Zero);

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(sprite.pivot(), Vec2f::Zero);
    ASSERT_EQ(sprite.position(), -Vec2f::One);
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));

    sprite.position(Vec2f::Zero);

    ASSERT_TRUE(is_stale(sprite, kStalePosition));
    ASSERT_TRUE(sprite.position().is_zero());

    sprite.update(vertex_array, mock_texture());

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(0, 0));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, 0));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(0, 2));
}

TEST(SpriteTest, Translates)
{
    Sprite sprite(2, 2);
    sprite.position(Vec2f::One);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, mock_texture());

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

    ASSERT_EQ(sprite.angle(), 0.0F);

    SpriteVertex vertex_array[4];
    for (int i = 0; i < 2; ++i)
    {
        const float scale = 1.0F + 0.5F * i;
        const float width = sprite.width() * scale;
        const float height = sprite.height() * scale;
        sprite.scale(scale);
        for (int deg = 0; deg <= 360; ++deg)
        {
            const float rad = rainbow::radians<float>(deg);
            sprite.angle(rad);

            ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
            ASSERT_EQ(sprite.angle(), rad);

            sprite.update(vertex_array, mock_texture());
            ASSERT_FALSE(is_stale(sprite));

            const std::array<Vec2f, 4> p{
                vertex_array[0].position,
                vertex_array[1].position,
                vertex_array[2].position,
                vertex_array[3].position,
            };

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

    sprite.angle(0.0F);
    sprite.scale(1.0F);
    sprite.update(vertex_array, mock_texture());

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

        sprite.update(vertex_array, mock_texture());
        ASSERT_FALSE(is_stale(sprite));

        const std::array<Vec2f, 4> p{
            vertex_array[0].position,
            vertex_array[1].position,
            vertex_array[2].position,
            vertex_array[3].position,
        };

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

    sprite.scale(2);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f(2.0F, 2.0F));

    SpriteVertex vertex_array[4];
    sprite.update(vertex_array, mock_texture());

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-2, -2));
    ASSERT_EQ(vertex_array[1].position, Vec2f(2, -2));
    ASSERT_EQ(vertex_array[2].position, Vec2f(2, 2));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-2, 2));

    sprite.scale(1);

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f::One);

    sprite.update(vertex_array, mock_texture());

    ASSERT_FALSE(is_stale(sprite));
    ASSERT_EQ(vertex_array[0].position, Vec2f(-1, -1));
    ASSERT_EQ(vertex_array[1].position, Vec2f(1, -1));
    ASSERT_EQ(vertex_array[2].position, Vec2f(1, 1));
    ASSERT_EQ(vertex_array[3].position, Vec2f(-1, 1));

    sprite.scale(Vec2f(1, 2));

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_EQ(sprite.scale(), Vec2f(1.0F, 2.0F));

    sprite.update(vertex_array, mock_texture());

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

    sprite.texture({0, 0, 1, 1});

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

    sprite.texture({0, 0, 1, 1});
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

    sprite.texture({0, 0, 1, 1});
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
    sprite.position(Vec2f::One);
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

    ASSERT_TRUE(is_stale(sprite, kStaleBuffer));
    ASSERT_TRUE(sprite.update(vertex_array, mock_texture()));
    ASSERT_FALSE(sprite.update(vertex_array, mock_texture()));

    sprite.move(Vec2f::Left);

    ASSERT_TRUE(is_stale(sprite, kStalePosition));
    ASSERT_TRUE(sprite.update(vertex_array, mock_texture()));
    ASSERT_FALSE(sprite.update(vertex_array, mock_texture()));
}

TEST(SpriteTest, ManuallyConstructedRefsAreInvalid)
{
    ASSERT_FALSE(SpriteRef{});
}
