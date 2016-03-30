// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Graphics/SpriteBatch.h"

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting {};
}

namespace
{
    rainbow::ISolemnlySwearThatIAmOnlyTesting mock;

    class SpriteFlipMirrorTest : public ::testing::Test
    {
    public:
        SpriteFlipMirrorTest()
            : atlas(make_shared<TextureAtlas>(mock)),
              texture(atlas->add_region(0, 0, 1, 1)),
              batch(mock),
              sprite(batch.create_sprite(2, 2)),
              vertex_array(sprite->vertex_array()) {}

    protected:
        SharedPtr<TextureAtlas> atlas;
        unsigned int texture;
        SpriteBatch batch;
        SpriteRef sprite = batch.create_sprite(2, 2);
        const SpriteVertex* vertex_array;
        Vec2f p[4];
        Vec2f u[4];

        void SetUp() override
        {
            batch.set_texture(atlas);
            sprite->set_texture(texture);
            sprite->update();
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
        SpriteHiddenTest() : batch(mock), sprite(batch.create_sprite(2, 2)) {}

    protected:
        SpriteBatch batch;
        SpriteRef sprite;
    };
}

TEST(SpriteTest, PlacedAtOriginOnCreation)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    ASSERT_EQ(Vec2f::Zero, sprite->position());

    sprite->update();

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f(-1, -1), vertex_array[0].position);
    ASSERT_EQ(Vec2f(1, -1), vertex_array[1].position);
    ASSERT_EQ(Vec2f(1, 1), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-1, 1), vertex_array[3].position);
}

TEST(SpriteTest, CanBeColored)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    const Colorb color(0xec, 0xef, 0xf1);
    ASSERT_NE(color, sprite->color());
    sprite->set_color(color);
    ASSERT_EQ(color, sprite->color());

    auto vertex_array = sprite->vertex_array();
    for (size_t i = 0; i < 4; ++i)
        ASSERT_EQ(color, vertex_array[i].color);
}

TEST(SpriteTest, CanBeTagged)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    ASSERT_EQ(Sprite::kNoId, sprite->id());

    const int tag = 0xdeadbeef;
    sprite->set_id(tag);
    ASSERT_EQ(tag, sprite->id());
}

TEST(SpriteTest, ChangesPivotWithoutShifting)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    ASSERT_EQ(Vec2f(0.5f, 0.5f), sprite->pivot());

    sprite->update();
    sprite->set_pivot(Vec2f::Zero);

    ASSERT_EQ(Vec2f::Zero, sprite->pivot());
    ASSERT_EQ(-Vec2f::One, sprite->position());

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f(-1, -1), vertex_array[0].position);
    ASSERT_EQ(Vec2f(1, -1), vertex_array[1].position);
    ASSERT_EQ(Vec2f(1, 1), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-1, 1), vertex_array[3].position);

    sprite->set_position(Vec2f::Zero);
    sprite->update();

    ASSERT_EQ(Vec2f::Zero, sprite->position());
    ASSERT_EQ(Vec2f(0, 0), vertex_array[0].position);
    ASSERT_EQ(Vec2f(2, 0), vertex_array[1].position);
    ASSERT_EQ(Vec2f(2, 2), vertex_array[2].position);
    ASSERT_EQ(Vec2f(0, 2), vertex_array[3].position);
}

TEST(SpriteTest, Translates)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    sprite->set_position(Vec2f::One);
    sprite->update();

    ASSERT_EQ(Vec2f::One, sprite->position());

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f(0, 0), vertex_array[0].position);
    ASSERT_EQ(Vec2f(2, 0), vertex_array[1].position);
    ASSERT_EQ(Vec2f(2, 2), vertex_array[2].position);
    ASSERT_EQ(Vec2f(0, 2), vertex_array[3].position);

    sprite->move(-Vec2f::One);

    ASSERT_EQ(Vec2f::Zero, sprite->position());
}

TEST(SpriteTest, Rotates)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(4, 2);

    ASSERT_EQ(0.0f, sprite->angle());

    auto vertex_array = sprite->vertex_array();
    for (int i = 0; i < 2; ++i)
    {
        const float scale = 1.0f + 0.5f * i;
        const float width = sprite->width() * scale;
        const float height = sprite->height() * scale;
        sprite->set_scale(scale);
        for (int deg = 0; deg <= 360; ++deg)
        {
            const float rad = rainbow::radians(deg);
            sprite->set_rotation(rad);

            ASSERT_EQ(rad, sprite->angle());

            sprite->update();
            const Vec2f p[]{vertex_array[0].position,
                            vertex_array[1].position,
                            vertex_array[2].position,
                            vertex_array[3].position};

            if (deg == 0 || deg == 360)
                ASSERT_FLOAT_EQ(scale + scale, p[2].x);
            else
                ASSERT_FALSE(rainbow::is_equal<float>(p[2].x, scale + scale));
            ASSERT_EQ(-p[2], p[0]);
            ASSERT_EQ(-p[3], p[1]);
            ASSERT_FLOAT_EQ(width, p[0].distance(p[1]));
            ASSERT_FLOAT_EQ(height, p[1].distance(p[2]));
            ASSERT_FLOAT_EQ(width, p[2].distance(p[3]));
            ASSERT_FLOAT_EQ(height, p[3].distance(p[0]));
        }
    }

    sprite->set_rotation(0.0f);
    sprite->set_scale(1.0f);
    sprite->update();

    const float width = sprite->width();
    const float height = sprite->height();

    float angle = 0;
    int i = 1;
    for (int deg = 1; deg <= 4; ++deg)
    {
        const float r = rainbow::radians(deg * i);
        angle += r;
        sprite->rotate(r);

        ASSERT_FLOAT_EQ(angle, sprite->angle());

        sprite->update();
        const Vec2f p[]{vertex_array[0].position,
                        vertex_array[1].position,
                        vertex_array[2].position,
                        vertex_array[3].position};

        ASSERT_EQ(-p[2], p[0]);
        ASSERT_EQ(-p[3], p[1]);
        ASSERT_FLOAT_EQ(width, p[0].distance(p[1]));
        ASSERT_FLOAT_EQ(height, p[1].distance(p[2]));
        ASSERT_FLOAT_EQ(width, p[2].distance(p[3]));
        ASSERT_FLOAT_EQ(height, p[3].distance(p[0]));

        i *= -1;
    }
}

TEST(SpriteTest, Scales)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    ASSERT_EQ(Vec2f::One, sprite->scale());

    sprite->set_scale(2);

    ASSERT_EQ(Vec2f(2.0f, 2.0f), sprite->scale());

    sprite->update();

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f(-2, -2), vertex_array[0].position);
    ASSERT_EQ(Vec2f(2, -2), vertex_array[1].position);
    ASSERT_EQ(Vec2f(2, 2), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-2, 2), vertex_array[3].position);

    sprite->set_scale(1);

    ASSERT_EQ(Vec2f::One, sprite->scale());

    sprite->update();

    ASSERT_EQ(Vec2f(-1, -1), vertex_array[0].position);
    ASSERT_EQ(Vec2f(1, -1), vertex_array[1].position);
    ASSERT_EQ(Vec2f(1, 1), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-1, 1), vertex_array[3].position);

    sprite->set_scale(Vec2f(1, 2));

    ASSERT_EQ(Vec2f(1.0f, 2.0f), sprite->scale());

    sprite->update();

    ASSERT_EQ(Vec2f(-1, -2), vertex_array[0].position);
    ASSERT_EQ(Vec2f(1, -2), vertex_array[1].position);
    ASSERT_EQ(Vec2f(1, 2), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-1, 2), vertex_array[3].position);
}

TEST_F(SpriteFlipMirrorTest, IsNotFlippedOrMirroredInitially)
{
    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(p[i], vertex_array[i].position);
        ASSERT_EQ(u[i], vertex_array[i].texcoord);
    }
}

TEST_F(SpriteFlipMirrorTest, FlipsVertically)
{
    sprite->flip();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[3], vertex_array[0].texcoord);
    ASSERT_EQ(u[2], vertex_array[1].texcoord);
    ASSERT_EQ(u[1], vertex_array[2].texcoord);
    ASSERT_EQ(u[0], vertex_array[3].texcoord);

    sprite->set_texture(texture);

    ASSERT_EQ(u[3], vertex_array[0].texcoord);
    ASSERT_EQ(u[2], vertex_array[1].texcoord);
    ASSERT_EQ(u[1], vertex_array[2].texcoord);
    ASSERT_EQ(u[0], vertex_array[3].texcoord);

    sprite->flip();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(p[i], vertex_array[i].position);
        ASSERT_EQ(u[i], vertex_array[i].texcoord);
    }
}

TEST_F(SpriteFlipMirrorTest, Mirrors)
{
    sprite->mirror();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_TRUE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[1], vertex_array[0].texcoord);
    ASSERT_EQ(u[0], vertex_array[1].texcoord);
    ASSERT_EQ(u[3], vertex_array[2].texcoord);
    ASSERT_EQ(u[2], vertex_array[3].texcoord);

    sprite->set_texture(texture);

    ASSERT_EQ(u[1], vertex_array[0].texcoord);
    ASSERT_EQ(u[0], vertex_array[1].texcoord);
    ASSERT_EQ(u[3], vertex_array[2].texcoord);
    ASSERT_EQ(u[2], vertex_array[3].texcoord);

    sprite->mirror();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(p[i], vertex_array[i].position);
        ASSERT_EQ(u[i], vertex_array[i].texcoord);
    }
}

TEST_F(SpriteFlipMirrorTest, FlipsAndMirrors)
{
    sprite->flip();
    sprite->mirror();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_TRUE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[2], vertex_array[0].texcoord);
    ASSERT_EQ(u[3], vertex_array[1].texcoord);
    ASSERT_EQ(u[0], vertex_array[2].texcoord);
    ASSERT_EQ(u[1], vertex_array[3].texcoord);

    sprite->set_texture(texture);

    ASSERT_EQ(u[2], vertex_array[0].texcoord);
    ASSERT_EQ(u[3], vertex_array[1].texcoord);
    ASSERT_EQ(u[0], vertex_array[2].texcoord);
    ASSERT_EQ(u[1], vertex_array[3].texcoord);

    sprite->flip();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_TRUE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[1], vertex_array[0].texcoord);
    ASSERT_EQ(u[0], vertex_array[1].texcoord);
    ASSERT_EQ(u[3], vertex_array[2].texcoord);
    ASSERT_EQ(u[2], vertex_array[3].texcoord);

    sprite->flip();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_TRUE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[2], vertex_array[0].texcoord);
    ASSERT_EQ(u[3], vertex_array[1].texcoord);
    ASSERT_EQ(u[0], vertex_array[2].texcoord);
    ASSERT_EQ(u[1], vertex_array[3].texcoord);

    sprite->mirror();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[3], vertex_array[0].texcoord);
    ASSERT_EQ(u[2], vertex_array[1].texcoord);
    ASSERT_EQ(u[1], vertex_array[2].texcoord);
    ASSERT_EQ(u[0], vertex_array[3].texcoord);

    sprite->flip();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(p[i], vertex_array[i].position);
        ASSERT_EQ(u[i], vertex_array[i].texcoord);
    }

    sprite->mirror();
    sprite->flip();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_TRUE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[2], vertex_array[0].texcoord);
    ASSERT_EQ(u[3], vertex_array[1].texcoord);
    ASSERT_EQ(u[0], vertex_array[2].texcoord);
    ASSERT_EQ(u[1], vertex_array[3].texcoord);

    sprite->mirror();

    ASSERT_TRUE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p[i], vertex_array[i].position);
    ASSERT_EQ(u[3], vertex_array[0].texcoord);
    ASSERT_EQ(u[2], vertex_array[1].texcoord);
    ASSERT_EQ(u[1], vertex_array[2].texcoord);
    ASSERT_EQ(u[0], vertex_array[3].texcoord);

    sprite->flip();

    ASSERT_FALSE(sprite->is_flipped());
    ASSERT_FALSE(sprite->is_mirrored());
    for (int i = 0; i < 4; ++i)
    {
        ASSERT_EQ(p[i], vertex_array[i].position);
        ASSERT_EQ(u[i], vertex_array[i].texcoord);
    }
}

TEST_F(SpriteHiddenTest, IgnoresMultipleCalls)
{
    ASSERT_FALSE(sprite->is_hidden());

    sprite->hide();
    ASSERT_TRUE(sprite->is_hidden());
    sprite->hide();
    ASSERT_TRUE(sprite->is_hidden());

    sprite->show();
    ASSERT_FALSE(sprite->is_hidden());
    sprite->show();
    ASSERT_FALSE(sprite->is_hidden());
}

TEST_F(SpriteHiddenTest, RestoresHiddenSprites)
{
    sprite->hide();
    sprite->update();

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f::Zero, vertex_array[0].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[1].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[2].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[3].position);

    sprite->show();
    sprite->update();

    ASSERT_EQ(Vec2f(-1, -1), vertex_array[0].position);
    ASSERT_EQ(Vec2f(1, -1), vertex_array[1].position);
    ASSERT_EQ(Vec2f(1, 1), vertex_array[2].position);
    ASSERT_EQ(Vec2f(-1, 1), vertex_array[3].position);
}

TEST_F(SpriteHiddenTest, CanBeMoved)
{
    sprite->hide();

    sprite->set_position(Vec2f::One);
    sprite->update();

    auto vertex_array = sprite->vertex_array();
    ASSERT_EQ(Vec2f::Zero, vertex_array[0].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[1].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[2].position);
    ASSERT_EQ(Vec2f::Zero, vertex_array[3].position);

    sprite->show();
    sprite->update();

    ASSERT_EQ(Vec2f(0, 0), vertex_array[0].position);
    ASSERT_EQ(Vec2f(2, 0), vertex_array[1].position);
    ASSERT_EQ(Vec2f(2, 2), vertex_array[2].position);
    ASSERT_EQ(Vec2f(0, 2), vertex_array[3].position);
}

TEST(SpriteTest, UpdatesOnlyOnChange)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto sprite = batch.create_sprite(2, 2);

    ASSERT_TRUE(sprite->update());
    ASSERT_FALSE(sprite->update());

    sprite->move(Vec2f::Left);

    ASSERT_TRUE(sprite->update());
    ASSERT_FALSE(sprite->update());
}

TEST(SpriteTest, ManuallyConstructedRefsAreInvalid)
{
    ASSERT_FALSE(SpriteRef{});
}
