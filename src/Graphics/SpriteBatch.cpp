// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

#include "Graphics/Driver.h"

using rainbow::SpriteBatch;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::TextureAtlas;
using rainbow::Vec2f;
using rainbow::graphics::Driver;

SpriteBatch::SpriteBatch(const Driver& driver, uint32_t count)
    : sprites_(count),
      vertices_(std::make_unique<SpriteVertex[]>(count * size_t{4})), count_(0),
      vertex_buffer_(driver.make_buffer<vk::VertexBuffer>(
          count * 4 * sizeof(SpriteVertex))),
      visible_(true)
{
    R_ASSERT(count <= vk::IndexBuffer::kMaxSprites, "Hard-coded limit reached");
}

SpriteBatch::SpriteBatch(SpriteBatch&& batch) noexcept
    : sprites_(std::move(batch.sprites_)),
      vertices_(std::move(batch.vertices_)), count_(batch.count_),
      vertex_buffer_(std::move(batch.vertex_buffer_)),
      texture_(std::move(batch.texture_)), visible_(batch.visible_)
{
    batch.clear();
}

void SpriteBatch::set_texture(std::shared_ptr<TextureAtlas> texture)
{
    texture_ = std::move(texture);
}

void SpriteBatch::bring_to_front(uint32_t i)
{
    sprites_.move(i, count_ - 1);
}

auto SpriteBatch::create_sprite(uint32_t width, uint32_t height) -> SpriteRef
{
    if (count_ == sprites_.size())
    {
        LOGW(
            "Tried to add a sprite (size: %ux%u) to a full sprite batch "
            "(capacity: %u). Increase the capacity and try again.",
            width,
            height,
            sprites_.size());
        return {};
    }

    new (sprites_.data() + count_) Sprite(width, height);
    const uint32_t offset = count_ * 4;
    std::fill_n(vertices_.get() + offset, 4, SpriteVertex{});
    return {*this, sprites_.find_iterator(count_++)};
}

void SpriteBatch::erase(uint32_t i)
{
    bring_to_front(i);
    sprites_.data()[--count_].~Sprite();
}

auto SpriteBatch::find_sprite_by_id(int id) const -> SpriteRef
{
    auto sprites = sprites_.data();
    for (uint32_t i = 0; i < count_; ++i)
    {
        if (sprites[i].id() == id)
            return {*const_cast<SpriteBatch*>(this), sprites_.find_iterator(i)};
    }

    return {};
}

void SpriteBatch::move(Vec2f delta)
{
    if (delta.is_zero())
        return;

    for (auto&& sprite : *this)
        sprite.move(delta);
}

void SpriteBatch::swap(uint32_t i, uint32_t j)
{
    if (i == j)
        return;

    sprites_.swap(i, j);
}

void SpriteBatch::update()
{
    bool needs_update = false;
    auto sprites = sprites_.data();
    for (uint32_t i = 0; i < count_; ++i)
    {
        ArraySpan<SpriteVertex> buffer{vertices_.get() + i * 4, 4};
        needs_update |= sprites[i].update(buffer, *texture_);
    }

    if (needs_update)
    {
        const uint32_t count = count_ * 4;
        vertex_buffer_.copy(ArraySpan<SpriteVertex>(vertices_.get(), count));
    }
}

#ifdef RAINBOW_TEST
SpriteBatch::SpriteBatch(const rainbow::ISolemnlySwearThatIAmOnlyTesting& test)
    : sprites_(4), vertices_(std::make_unique<SpriteVertex[]>(4 * 4)),
      count_(0), vertex_buffer_(test),
      texture_(std::make_shared<TextureAtlas>(test)), visible_(true)
{
    texture_->add_region(0, 0, 1, 1);
}
#endif  // RAINBOW_TEST

void rainbow::vk::draw(const CommandBuffer& command_buffer,
                       const SpriteBatch& sprite_batch,
                       const IndexBuffer& index_buffer)
{
    const auto vertex_count = sprite_batch.vertex_count();
    if (vertex_count == 0)
        return;

    update_descriptor(command_buffer, sprite_batch.texture());
    draw(command_buffer,
         sprite_batch.vertex_buffer(),
         vertex_count,
         index_buffer);
}
