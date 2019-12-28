// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Collision/SAT.h"

#include <gtest/gtest.h>

#include "Graphics/SpriteVertex.h"
#include "Math/Geometry.h"
#include "Math/Transform.h"
#include "Math/Vec2.h"
#include "Tests/TestHelpers.h"

using rainbow::Quad;
using rainbow::SpriteVertex;
using rainbow::Vec2f;

namespace
{
    constexpr float kSize = 10.0F;

    struct Hitbox
    {
        Quad quad{
            {-5.0F, -5.0F},
            {5.0F, -5.0F},
            {5.0F, 5.0F},
            {-5.0F, 5.0F},
        };
        float r = 0.0F;

        void move(Vec2f distance)
        {
            quad.v0 += distance;
            quad.v1 += distance;
            quad.v2 += distance;
            quad.v3 += distance;
        }

        void position(Vec2f position)
        {
            quad = Hitbox{}.quad;
            transform();
            move(position - Vec2f::Zero);
        }

        void rotate(float radian)
        {
            r += radian;
            transform();
        }

        void transform()
        {
            SpriteVertex out[4];
            rainbow::transform(
                {
                    quad.v0,
                    quad.v1,
                    quad.v2,
                    quad.v3,
                },
                {
                    kSize * 0.5F + quad.v0.x,
                    kSize * 0.5F + quad.v0.y,
                },
                r,
                Vec2f::One,
                ArraySpan<SpriteVertex>{out});
            quad.v0 = out[0].position;
            quad.v1 = out[1].position;
            quad.v2 = out[2].position;
            quad.v3 = out[3].position;
        }
    };
}  // namespace

TEST(CollisionTest, SeparatingAxisTheorem)
{
    Hitbox b0, b1;

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-kSize, 0});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower left corner

    b1.move({0, -kSize});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, 0});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, -1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower right corner

    b1.move({kSize * 2, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, 1});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper right corner

    b1.move({0, kSize * 2});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 0});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, 1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper left corner

    b1.move({-kSize * 2, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, -1});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // B0: 0° / B1: 45°

    b1 = Hitbox{};
    b1.rotate(rainbow::radians(45.0F));

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Left

    const float diagonal = std::sqrt(kSize * kSize + kSize * kSize);
    const float distance = kSize * 0.5F + diagonal * 0.5F;
    b1.move({-distance, 0});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower left corner

    const float inner_size = kSize * 0.5F + std::sqrt(kSize * kSize / 8.0F);
    b1.position({-inner_size, -inner_size});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, -1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Bottom

    b1.position({0, -distance});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, -1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower right corner

    b1.position({inner_size, -inner_size});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, -1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, 1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Right

    b1.position({distance, 0});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper right corner

    b1.position({inner_size, inner_size});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({1, 0});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, 1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Top

    b1.position({0, distance});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, 1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper left corner

    b1.position({-inner_size, inner_size});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0, 1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-1, -1});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower left quadrant

    b0.rotate(rainbow::radians(45.0F));
    b1.position({diagonal * -0.5F, diagonal * -0.5F});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-0.1F, -0.1F});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Lower right quadrant

    b1.position({diagonal * 0.5F, diagonal * -0.5F});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0.1F, -0.1F});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper right quadrant

    b1.position({diagonal * 0.5F, diagonal * 0.5F});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({0.1F, 0.1F});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    // Upper left quadrant

    b1.position({-diagonal * 0.5F, diagonal * 0.5F});

    ASSERT_TRUE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));

    b1.move({-0.1F, 0.1F});

    ASSERT_FALSE(rainbow::overlaps(b0.quad, b0.r, b1.quad, b1.r));
}
