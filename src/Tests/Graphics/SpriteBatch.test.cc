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
    template <size_t N>
    void set_sprite_ids(SpriteRef (&refs)[N])
    {
        for (size_t i = 0; i < N; ++i)
            refs[i]->set_id(i + 1);

        for (size_t i = 0; i < N; ++i)
            ASSERT_EQ(i + 1, static_cast<size_t>(refs[i]->id()));
    }

    void update(SpriteBatch& batch)
    {
        auto sprites = batch.sprites();
        for (uint32_t i = 0; i < batch.size(); ++i)
        {
            ArraySpan<SpriteVertex> buffer{batch.vertices() + i * 4, 4};
            sprites[i].update(buffer, batch.texture());
        }
    }

    void verify_sprite_vertices(const Sprite& sprite,
                                const SpriteVertex* vertices,
                                const Vec2f& offset)
    {
        const float half_width = sprite.width() * 0.5f;
        const float half_height = sprite.height() * 0.5f;
        ASSERT_EQ(Vec2f(-half_width + offset.x, -half_height + offset.y),
                  vertices[0].position);
        ASSERT_EQ(Vec2f(half_width + offset.x, -half_height + offset.y),
                  vertices[1].position);
        ASSERT_EQ(Vec2f(half_width + offset.x, half_height + offset.y),
                  vertices[2].position);
        ASSERT_EQ(Vec2f(-half_width + offset.x, half_height + offset.y),
                  vertices[3].position);
    }

    void verify_sprites_vertices(const SpriteBatch& batch)
    {
        for (auto&& sprite : batch)
            verify_sprite_vertices(sprite, sprite.vertex_array(), Vec2f::Zero);
    }

    void verify_batch_integrity(const SpriteBatch& batch)
    {
        char trace[16];
        const auto sprites = batch.sprites();
        const SpriteVertex* vertices = batch.vertices();
        for (uint32_t i = 0; i < batch.size(); ++i)
        {
            snprintf(trace, sizeof(trace), "i = %u", i);
            SCOPED_TRACE(trace);
            auto vertex_array = vertices + i * 4;
            ASSERT_EQ(vertex_array, sprites[i].vertex_array());
            verify_sprite_vertices(sprites[i], vertex_array, Vec2f::Zero);
        }
    }

    class SpriteBatchOperationsTest : public ::testing::Test
    {
    public:
        SpriteBatchOperationsTest()
            : batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{}),
              refs{batch.create_sprite(1, 1), batch.create_sprite(2, 1),
                   batch.create_sprite(1, 2), batch.create_sprite(2, 2)},
              count(rainbow::array_size(refs)),
              vertices(batch.vertices()) {}

    protected:
        SpriteBatch batch;
        SpriteRef refs[4];
        const size_t count;
        const SpriteVertex* vertices;

        void SetUp() override { update(batch); }
    };
}

TEST(SpriteBatchTest, MoveConstructs)
{
    rainbow::ISolemnlySwearThatIAmOnlyTesting mock;

    SpriteBatch batch(mock);
    auto atlas = make_shared<TextureAtlas>(mock);
    batch.set_texture(atlas);

    for (uint32_t i = 0; i < batch.capacity(); ++i)
        batch.create_sprite(i + 1, i + 1);
    update(batch);

    const uint32_t vertex_count = batch.vertex_count();
    const Sprite* sprites_array = batch.sprites();
    const TextureAtlas& texture = batch.texture();
    const uint32_t sprite_count = batch.size();
    const SpriteVertex* vertices = batch.vertices();

    SpriteBatch moved(std::move(batch));

    ASSERT_EQ(vertex_count, moved.vertex_count());
    ASSERT_EQ(sprites_array, moved.sprites());
    ASSERT_EQ(&texture, &moved.texture());
    ASSERT_EQ(sprite_count, moved.size());
    ASSERT_EQ(vertices, moved.vertices());

    for (uint32_t i = 0; i < moved.size(); ++i)
    {
        const float s = (i + 1) * 0.5f;
        const uint32_t j = i * 4;
        ASSERT_EQ(Vec2f(-s, -s), vertices[j].position);
        ASSERT_EQ(Vec2f(s, -s), vertices[j + 1].position);
        ASSERT_EQ(Vec2f(s, s), vertices[j + 2].position);
        ASSERT_EQ(Vec2f(-s, s), vertices[j + 3].position);
    }

    ASSERT_EQ(nullptr, batch.sprites());
    ASSERT_EQ(nullptr, batch.vertices());
    ASSERT_EQ(0u, batch.vertex_count());
}

TEST(SpriteBatchTest, CreatesSprites)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    SpriteRef sprites[]{
        batch.create_sprite(1, 1),
        batch.create_sprite(2, 2),
        batch.create_sprite(3, 3),
        batch.create_sprite(4, 4),
    };
    set_sprite_ids(sprites);
    update(batch);

    ASSERT_EQ(1u, sprites[0]->width());
    ASSERT_EQ(2u, sprites[1]->width());
    ASSERT_EQ(3u, sprites[2]->width());
    ASSERT_EQ(4u, sprites[3]->width());

    verify_batch_integrity(batch);

    batch.erase(sprites[2]);

    ASSERT_EQ(1u, sprites[0]->width());
    ASSERT_EQ(2u, sprites[1]->width());
    ASSERT_EQ(4u, sprites[3]->width());

    sprites[2] = batch.create_sprite(7, 7);

    ASSERT_EQ(1u, sprites[0]->width());
    ASSERT_EQ(2u, sprites[1]->width());
    ASSERT_EQ(7u, sprites[2]->width());
    ASSERT_EQ(4u, sprites[3]->width());

    update(batch);

    verify_batch_integrity(batch);
}

TEST(SpriteBatchTest, IsVisible)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    batch.create_sprite(1, 1);

    ASSERT_TRUE(batch.is_visible());
    ASSERT_EQ(6u, batch.vertex_count());

    batch.set_visible(true);

    ASSERT_TRUE(batch.is_visible());
    ASSERT_EQ(6u, batch.vertex_count());

    batch.set_visible(false);

    ASSERT_FALSE(batch.is_visible());
    ASSERT_EQ(0u, batch.vertex_count());

    batch.set_visible(false);

    ASSERT_FALSE(batch.is_visible());
    ASSERT_EQ(0u, batch.vertex_count());

    batch.set_visible(true);

    ASSERT_TRUE(batch.is_visible());
    ASSERT_EQ(6u, batch.vertex_count());

    SpriteBatch batch2(std::move(batch));

    ASSERT_TRUE(batch2.is_visible());
    ASSERT_EQ(6u, batch2.vertex_count());

    batch2.set_visible(false);
    SpriteBatch batch3(std::move(batch2));

    ASSERT_FALSE(batch3.is_visible());
    ASSERT_EQ(0u, batch3.vertex_count());
}

TEST(SpriteBatchTest, ExcessSpritesAreDiscarded)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    uint32_t capacity = batch.capacity();
    for (uint32_t i = 0; i < capacity; ++i)
        batch.create_sprite(i + 1, i + 1);
    update(batch);

    ASSERT_EQ(capacity, batch.size());

    auto&& vertices = batch.vertices();
    for (uint32_t i = 0; i < batch.size(); ++i)
    {
        const float s = (i + 1) * 0.5f;
        const uint32_t j = i * 4;
        ASSERT_EQ(Vec2f(-s, -s), vertices[j].position);
        ASSERT_EQ(Vec2f(s, -s), vertices[j + 1].position);
        ASSERT_EQ(Vec2f(s, s), vertices[j + 2].position);
        ASSERT_EQ(Vec2f(-s, s), vertices[j + 3].position);
    }

    batch.create_sprite(capacity + 1, capacity + 1);

    ASSERT_EQ(capacity, batch.size());

    for (uint32_t i = 0; i < batch.size(); ++i)
    {
        const float s = (i + 1) * 0.5f;
        const uint32_t j = i * 4;
        ASSERT_EQ(Vec2f(-s, -s), vertices[j].position);
        ASSERT_EQ(Vec2f(s, -s), vertices[j + 1].position);
        ASSERT_EQ(Vec2f(s, s), vertices[j + 2].position);
        ASSERT_EQ(Vec2f(-s, s), vertices[j + 3].position);
    }

    batch.create_sprite(capacity + 1, capacity + 1);
    batch.create_sprite(capacity + 1, capacity + 1);
    batch.create_sprite(capacity + 1, capacity + 1);

    ASSERT_EQ(capacity, batch.size());

    for (uint32_t i = 0; i < batch.size(); ++i)
    {
        const float s = (i + 1) * 0.5f;
        const uint32_t j = i * 4;
        ASSERT_EQ(Vec2f(-s, -s), vertices[j].position);
        ASSERT_EQ(Vec2f(s, -s), vertices[j + 1].position);
        ASSERT_EQ(Vec2f(s, s), vertices[j + 2].position);
        ASSERT_EQ(Vec2f(-s, s), vertices[j + 3].position);
    }
}

TEST(SpriteBatchTest, DISABLED_ExpandsToAccommodateNewSprites)
{
    SpriteBatch batch(rainbow::ISolemnlySwearThatIAmOnlyTesting{});
    uint32_t capacity = batch.capacity();
    for (uint32_t i = 0; i < capacity; ++i)
        batch.create_sprite(i + 1, i + 1);
    update(batch);

    ASSERT_EQ(capacity, batch.size());

    batch.create_sprite(capacity + 1, capacity + 1);
    update(batch);

    ASSERT_LT(batch.size(), batch.capacity());
    ASSERT_GT(batch.capacity(), capacity);

    auto&& vertices = batch.vertices();
    for (uint32_t i = 0; i < batch.size(); ++i)
    {
        const float s = (i + 1) * 0.5f;
        const uint32_t j = i * 4;
        ASSERT_EQ(Vec2f(-s, -s), vertices[j].position);
        ASSERT_EQ(Vec2f(s, -s), vertices[j + 1].position);
        ASSERT_EQ(Vec2f(s, s), vertices[j + 2].position);
        ASSERT_EQ(Vec2f(-s, s), vertices[j + 3].position);
    }
}

TEST_F(SpriteBatchOperationsTest, SpritesShareASingleBuffer)
{
    ASSERT_EQ(count * 6, batch.vertex_count());

    const auto sprites = batch.sprites();
    for (size_t i = 0; i < count; ++i)
    {
        ASSERT_EQ(&sprites[i], &(*refs[i]));
        ASSERT_EQ(vertices + i * 4, refs[i]->vertex_array());
    }
}

TEST_F(SpriteBatchOperationsTest, SpritesPositionAtOriginOnCreation)
{
    update(batch);
    for (size_t i = 0; i < count; ++i)
    {
        ASSERT_EQ(Vec2f::Zero, refs[i]->position());
        verify_sprite_vertices(*refs[i], vertices + i * 4, Vec2f::Zero);
    }
}

TEST_F(SpriteBatchOperationsTest, BringsSpritesToFront)
{
    set_sprite_ids(refs);
    update(batch);

    batch.bring_to_front(refs[1]);

    ASSERT_EQ(1, batch[0].id());
    ASSERT_EQ(2, batch[1].id());
    ASSERT_EQ(3, batch[2].id());
    ASSERT_EQ(4, batch[3].id());

    auto sprites = batch.sprites();

    ASSERT_EQ(1, sprites[0].id());
    ASSERT_EQ(3, sprites[1].id());
    ASSERT_EQ(4, sprites[2].id());
    ASSERT_EQ(2, sprites[3].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);
}

TEST_F(SpriteBatchOperationsTest, ClearsSprites)
{
    const auto capacity = batch.capacity();

    ASSERT_EQ(count, batch.size());

    batch.clear();

    ASSERT_EQ(0u, batch.vertex_count());
    ASSERT_EQ(0u, batch.size());
    ASSERT_EQ(capacity, batch.capacity());
}

TEST_F(SpriteBatchOperationsTest, ErasesSprites)
{
    set_sprite_ids(refs);
    update(batch);

    batch.erase(batch.find_sprite_by_id(2));

    size_t length = count - 1;
    ASSERT_EQ(length * 6, batch.vertex_count());

    auto sprites = batch.sprites();
    ASSERT_EQ(1, sprites[0].id());
    ASSERT_EQ(3, sprites[1].id());
    ASSERT_EQ(4, sprites[2].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);

    batch.erase(batch.find_sprite_by_id(1));

    ASSERT_EQ(--length * 6, batch.vertex_count());
    ASSERT_EQ(3, sprites[0].id());
    ASSERT_EQ(4, sprites[1].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);

    batch.erase(batch.find_sprite_by_id(4));
    update(batch);

    ASSERT_EQ(6u, batch.vertex_count());
    ASSERT_EQ(3, sprites[0].id());
    ASSERT_EQ(vertices, sprites[0].vertex_array());
    verify_sprite_vertices(sprites[0], vertices, Vec2f::Zero);

    batch.erase(batch.find_sprite_by_id(3));

    ASSERT_EQ(0u, batch.vertex_count());
}

TEST_F(SpriteBatchOperationsTest, FindsSpritesById)
{
    set_sprite_ids(refs);

    for (auto&& sprite : refs)
        ASSERT_EQ(&(*batch.find_sprite_by_id(sprite->id())), &(*sprite));

    ASSERT_FALSE(batch.find_sprite_by_id(0xdeadbeef));
}

TEST_F(SpriteBatchOperationsTest, MovesSprites)
{
    batch.move(Vec2f::One);
    update(batch);
    for (size_t i = 0; i < count; ++i)
        verify_sprite_vertices(*refs[i], vertices + i * 4, Vec2f::One);

    batch.move(-Vec2f::One);
    update(batch);

    verify_batch_integrity(batch);
}

TEST_F(SpriteBatchOperationsTest, SwapsSprites)
{
    set_sprite_ids(refs);
    update(batch);

    batch.swap(refs[0], refs[0]);

    auto sprites = batch.sprites();
    ASSERT_EQ(1, sprites[0].id());
    ASSERT_EQ(2, sprites[1].id());
    ASSERT_EQ(3, sprites[2].id());
    ASSERT_EQ(4, sprites[3].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);

    batch.swap(refs[0], refs[3]);

    ASSERT_EQ(4, sprites[0].id());
    ASSERT_EQ(2, sprites[1].id());
    ASSERT_EQ(3, sprites[2].id());
    ASSERT_EQ(1, sprites[3].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);

    batch.swap(refs[1], refs[2]);

    ASSERT_EQ(4, sprites[0].id());
    ASSERT_EQ(3, sprites[1].id());
    ASSERT_EQ(2, sprites[2].id());
    ASSERT_EQ(1, sprites[3].id());

    verify_sprites_vertices(batch);

    update(batch);

    verify_batch_integrity(batch);
}
