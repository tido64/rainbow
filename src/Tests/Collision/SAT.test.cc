// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Collision/SAT.h"
#include "Graphics/SpriteBatch.h"

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting {};
}

namespace
{
    void update(SpriteBatch& batch)
    {
        auto sprites = batch.sprites();
        for (uint32_t i = 0; i < batch.size(); ++i)
        {
            ArraySpan<SpriteVertex> buffer{batch.vertices() + i * 4, 4};
            sprites[i].update(buffer, batch.texture());
        }
    }
}

TEST(CollisionTest, SeparatingAxisTheorem)
{
    constexpr int size = 10;

    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    auto s0 = batch.create_sprite(size, size);
    auto s1 = batch.create_sprite(size, size);
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-size, 0});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower left corner

    s1->move({0, -size});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({1, 0});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, -1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower right corner

    s1->move({size * 2, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({0, 1});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper right corner

    s1->move({0, size * 2});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({-1, 0});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, 1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper left corner

    s1->move({-size * 2, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({0, -1});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // S0: 0° / S1: 45°

    s1->set_position(Vec2f::Zero);
    s1->rotate(rainbow::radians(45.0f));
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    // Left

    const float diagonal = std::sqrt(size * size + size * size);
    const float distance = size * 0.5f + diagonal * 0.5f;
    s1->move({-distance, 0});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower left corner

    const float inner_size = size * 0.5f + std::sqrt(size * size / 8.0f);
    s1->set_position({-inner_size, -inner_size});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({1, -1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Bottom

    s1->set_position({0, -distance});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, -1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower right corner

    s1->set_position({inner_size, -inner_size});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, -1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({1, 1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Right

    s1->set_position({distance, 0});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper right corner

    s1->set_position({inner_size, inner_size});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({1, 0});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({-1, 1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Top

    s1->set_position({0, distance});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, 1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper left corner

    s1->set_position({-inner_size, inner_size});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0, 1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    s1->move({-1, -1});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower left quadrant

    s0->rotate(rainbow::radians(45.0f));
    s1->set_position({diagonal * -0.5f, diagonal * -0.5f});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-0.1f, -0.1f});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Lower right quadrant

    s1->set_position({diagonal * 0.5f, diagonal * -0.5f});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0.1f, -0.1f});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper right quadrant

    s1->set_position({diagonal * 0.5f, diagonal * 0.5f});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({0.1f, 0.1f});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));

    // Upper left quadrant

    s1->set_position({-diagonal * 0.5f, diagonal * 0.5f});
    update(batch);

    ASSERT_TRUE(rainbow::overlaps(s0, s1));

    s1->move({-0.1f, 0.1f});
    update(batch);

    ASSERT_FALSE(rainbow::overlaps(s0, s1));
}
