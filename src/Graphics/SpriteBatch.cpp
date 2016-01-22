// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

#include <algorithm>

#include "Graphics/Renderer.h"
#include "Graphics/ShaderDetails.h"

namespace
{
    template <typename T>
    class SetBuffer
    {
    public:
        explicit SetBuffer(T* buffer) : buffer_(buffer) {}

        void operator()(Sprite& sprite);

    private:
        T* buffer_;
    };

    class Update
    {
    public:
        Update() : should_update_(false) {}

        void operator()(Sprite& sprite) { should_update_ |= sprite.update(); }

        explicit operator bool() const { return should_update_; }

    private:
        bool should_update_;
    };

    template <>
    void SetBuffer<SpriteVertex>::operator()(Sprite& sprite)
    {
        sprite.set_vertex_array(buffer_);
        buffer_ += 4;
    }

    template <>
    void SetBuffer<Vec2f>::operator()(Sprite& sprite)
    {
        sprite.set_normal_buffer(buffer_);
        buffer_ += 4;
    }
}

SpriteBatch::SpriteBatch(unsigned int hint)
    : count_(0), reserved_(0), visible_(true)
{
    resize(hint);
    array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
}

SpriteBatch::SpriteBatch(const rainbow::ISolemnlySwearThatIAmOnlyTesting& test)
    : count_(0), vertex_buffer_(test), normal_buffer_(test), reserved_(0),
      visible_(true)
{
    resize(4);
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
        std::uninitialized_fill_n(normals_.get(), count_ * 4, Vec2f());
        set_buffer(normals_.get());
        array_.reconfigure(std::bind(&SpriteBatch::bind_arrays, this));
    }
    normal_ = std::move(texture);
}

void SpriteBatch::set_texture(SharedPtr<TextureAtlas> texture)
{
    texture_ = std::move(texture);
}

Sprite::Ref SpriteBatch::add(int x, int y, int w, int h)
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

void SpriteBatch::bring_to_front(const Sprite::Ref& s)
{
    R_ASSERT(s.batch_ == this, "Sprite does not belong to this batch");

    rotate(s.i_, s.i_ + 1, count_);
}

Sprite::Ref SpriteBatch::create_sprite(unsigned int width, unsigned int height)
{
    R_ASSERT(count_ <= Renderer::kNumSprites, "Hard-coded limit reached");

    if (count_ == reserved_)
    {
        const unsigned int half = reserved_ / 2;
        resize(reserved_ + (half == 0 ? 1 : half));
    }
    Sprite& sprite = sprites_[count_];
    new (&sprite) Sprite(width, height, this);
    const unsigned int offset = count_ * 4;
    std::uninitialized_fill_n(vertices_ + offset, 4, SpriteVertex());
    sprite.set_vertex_array(vertices_ + offset);
    if (normals_)
    {
        std::uninitialized_fill_n(normals_ + offset, 4, Vec2f());
        sprite.set_normal_buffer(normals_ + offset);
    }
    return Sprite::Ref(this, count_++);
}

void SpriteBatch::erase(const Sprite::Ref& s)
{
    if (s.i_ + 1 < count_)
        bring_to_front(s);
    sprites_[--count_].~Sprite();
}

Sprite::Ref SpriteBatch::find_sprite_by_id(int id) const
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

void SpriteBatch::swap(const Sprite::Ref& a_ref, const Sprite::Ref& b_ref)
{
    if (a_ref == b_ref)
        return;

    // Swap the offsets first as refs are invalidated after.
    const size_t offset_a = b_ref.i_ * 4;
    const size_t offset_b = a_ref.i_ * 4;

    Sprite& a = *a_ref;
    Sprite& b = *b_ref;
    std::swap(a, b);

    auto buf_a = vertices_ + offset_a;
    auto buf_b = vertices_ + offset_b;
    a.set_vertex_array(buf_b);
    b.set_vertex_array(buf_a);
    std::swap_ranges(buf_a, buf_a + 4, buf_b);

    if (normals_)
    {
        auto buf_a = normals_ + offset_a;
        auto buf_b = normals_ + offset_b;
        a.set_normal_buffer(buf_b);
        b.set_normal_buffer(buf_a);
        std::swap_ranges(buf_a, buf_a + 4, buf_b);
    }
}

void SpriteBatch::update()
{
    // Update all sprites, then upload the vertex buffer if necessary.
    if (std::for_each(sprites_.get(), sprites_ + count_, Update()))
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
    set_buffer(vertices_.get());
    if (normals_)
    {
        normals_.resize(count_ * 4, size * 4);
        set_buffer(normals_.get());
    }
    reserved_ = size;
}

void SpriteBatch::rotate(size_t first, size_t n_first, size_t last)
{
    R_ASSERT(first < count_, "Index out of bounds");
    R_ASSERT(n_first < count_, "Index out of bounds");
    R_ASSERT(last <= count_, "Index out of bounds");

    std::rotate(sprites_ + first, sprites_ + n_first, sprites_ + last);
    std::for_each(sprites_ + first,
                  sprites_ + last,
                  SetBuffer<SpriteVertex>(vertices_ + first * 4));
    if (normals_)
    {
        std::for_each(sprites_ + first,
                      sprites_ + last,
                      SetBuffer<Vec2f>(normals_ + first * 4));
    }

    first *= 4;
    n_first *= 4;
    last *= 4;

    std::rotate(vertices_ + first, vertices_ + n_first, vertices_ + last);
    if (normals_)
        std::rotate(normals_ + first, normals_ + n_first, normals_ + last);
}

template <typename T>
void SpriteBatch::set_buffer(T* buffer)
{
    std::for_each(sprites_.get(), sprites_ + count_, SetBuffer<T>(buffer));
}
