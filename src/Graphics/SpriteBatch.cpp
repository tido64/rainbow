// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

#include <algorithm>

#include "Graphics/Renderer.h"
#include "Graphics/ShaderDetails.h"

SpriteBatch::SpriteBatch(unsigned int hint)
    : count_(0), reserved_(0), visible_(true)
{
    resize(hint);
    array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
}

SpriteBatch::SpriteBatch(SharedPtr<TextureAtlas> texture, SpriteList sprites)
    : SpriteBatch(sprites.size())
{
    set_texture(std::move(texture));
    for (auto&& s : sprites)
    {
        auto sprite = create_sprite(s.size().x, s.size().y);
        sprite->set_position(s.position());
        sprite->set_texture(s.texture());
        sprite->set_color(s.color());
        sprite->set_rotation(s.angle());
        sprite->set_pivot(s.pivot());
        sprite->set_scale(s.scale());
        if (s.ref() != nullptr)
            *s.ref() = sprite;
    }
}

SpriteBatch::SpriteBatch(SpriteBatch&& batch)
    : sprites_(std::move(batch.sprites_)),
      vertices_(std::move(batch.vertices_)),
      normals_(std::move(batch.normals_)), count_(batch.count_),
      vertex_buffer_(std::move(batch.vertex_buffer_)),
      normal_buffer_(std::move(batch.normal_buffer_)),
      array_(std::move(batch.array_)), normal_(std::move(batch.normal_)),
      texture_(std::move(batch.texture_)), reserved_(batch.reserved_),
      visible_(batch.visible_)
{
    batch.clear();
}

SpriteBatch::~SpriteBatch()
{
    sprites_.release(count_);
    vertices_.release(count_ * 4);
    normals_.release(count_ * 4);
}

void SpriteBatch::set_normal(SharedPtr<TextureAtlas> texture)
{
    if (!normals_)
    {
        normals_.resize(0, reserved_ * 4);
        std::uninitialized_fill_n(normals_.get(), count_ * 4, Vec2f{});
        array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
    }

    normal_ = std::move(texture);
}

void SpriteBatch::set_texture(SharedPtr<TextureAtlas> texture)
{
    texture_ = std::move(texture);
}

SpriteRef SpriteBatch::add(int x, int y, int w, int h)
{
    auto sprite = create_sprite(w, h);
    sprite->set_texture(texture_->add_region(x, y, w, h));
    return sprite;
}

void SpriteBatch::bind_textures() const
{
    if (normal_)
        normal_->bind(1);
    texture_->bind();
}

void SpriteBatch::bring_to_front(const SpriteRef& s)
{
    R_ASSERT(s.batch_ == this, "Sprite does not belong to this batch");

    rotate(s.i_, s.i_ + 1, count_);
}

SpriteRef SpriteBatch::create_sprite(unsigned int width, unsigned int height)
{
    R_ASSERT(count_ <= rainbow::graphics::kMaxSprites,
             "Hard-coded limit reached");

    if (count_ == reserved_)
    {
        const unsigned int half = reserved_ / 2;
        resize(reserved_ + (half == 0 ? 1 : half));
    }

    Sprite& sprite = sprites_[count_];
    new (&sprite) Sprite(width, height);
    const unsigned int offset = count_ * 4;
    std::uninitialized_fill_n(vertices_ + offset, 4, SpriteVertex{});
    if (normals_)
        std::uninitialized_fill_n(normals_ + offset, 4, Vec2f{});
    return {this, count_++};
}

void SpriteBatch::erase(const SpriteRef& s)
{
    if (s.i_ + 1 < count_)
        bring_to_front(s);
    sprites_[--count_].~Sprite();
}

SpriteRef SpriteBatch::find_sprite_by_id(int id) const
{
    for (unsigned int i = 0; i < count_; ++i)
    {
        if (sprites_[i].id() == id)
            return {this, i};
    }
    return {};
}

void SpriteBatch::move(const Vec2f& delta)
{
    if (delta.is_zero())
        return;

    for (auto&& sprite : *this)
        sprite.move(delta);
}

void SpriteBatch::swap(const SpriteRef& a_ref, const SpriteRef& b_ref)
{
    if (a_ref == b_ref)
        return;

    std::swap(*a_ref, *b_ref);
}

void SpriteBatch::update()
{
    bool needs_update = false;
    if (normals_)
    {
        for (unsigned int i = 0; i < count_; ++i)
        {
            needs_update |=
                sprites_[i].update(
                    ArraySpan<Vec2f>(normals_ + i * 4, 4), *normal_) |
                sprites_[i].update(
                    ArraySpan<SpriteVertex>(vertices_ + i * 4, 4), *texture_);
        }
    }
    else
    {
        for (unsigned int i = 0; i < count_; ++i)
        {
            needs_update |= sprites_[i].update(
                ArraySpan<SpriteVertex>(vertices_ + i * 4, 4), *texture_);
        }
    }

    if (needs_update)
    {
        const unsigned int count = count_ * 4;
        vertex_buffer_.upload(vertices_.get(), count * sizeof(SpriteVertex));
        if (normals_)
            normal_buffer_.upload(normals_.get(), count * sizeof(Vec2f));
    }
}

void SpriteBatch::bind_arrays() const
{
    vertex_buffer_.bind();
    if (normals_)
        normal_buffer_.bind(Shader::kAttributeNormal);
}

void SpriteBatch::resize(unsigned int size)
{
    sprites_.resize(count_, size);
    vertices_.resize(count_ * 4, size * 4);
    if (normals_)
        normals_.resize(count_ * 4, size * 4);
    reserved_ = size;
}

void SpriteBatch::rotate(size_t first, size_t n_first, size_t last)
{
    R_ASSERT(first < count_, "Index out of bounds");
    R_ASSERT(n_first < count_, "Index out of bounds");
    R_ASSERT(last <= count_, "Index out of bounds");

    std::rotate(sprites_ + first, sprites_ + n_first, sprites_ + last);
}

#ifdef RAINBOW_TEST
SpriteBatch::SpriteBatch(const rainbow::ISolemnlySwearThatIAmOnlyTesting& test)
    : count_(0), vertex_buffer_(test), normal_buffer_(test),
      texture_(make_shared<TextureAtlas>(test)), reserved_(0), visible_(true)
{
    resize(4);
    texture_->add_region(0, 0, 1, 1);
}
#endif  // RAINBOW_TEST
