// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/SpriteBatch.h"

#include "Math/Transform.h"

namespace
{
    const unsigned int kStaleBuffer       = 1u << 0;
    const unsigned int kStalePosition     = 1u << 1;
    const unsigned int kStaleFrontBuffer  = 1u << 2;
    const unsigned int kStaleMask         = 0xffffu;
    const unsigned int kIsHidden          = 1u << 16;
    const unsigned int kIsFlipped         = 1u << 17;
    const unsigned int kIsMirrored        = 1u << 18;

    const unsigned int kFlipTable[]{
        0, 1, 2, 3,   // Normal
        3, 2, 1, 0,   // Flipped
        1, 0, 3, 2,   // Mirrored
        2, 3, 0, 1};  // Flipped + mirrored
    const unsigned int kFlipVertically = 4;
    const unsigned int kFlipHorizontally = 8;

    static_assert((kIsFlipped >> 0xf) == kFlipVertically, "");
    static_assert((kIsMirrored >> 0xf) == kFlipHorizontally, "");

    unsigned int flip_index(unsigned int state)
    {
        return ((state & kIsFlipped) >> 0xf) + ((state & kIsMirrored) >> 0xf);
    }
}

Sprite& SpriteRef::operator*() const
{
    return batch_->sprites()[i_];
}

Sprite* SpriteRef::operator->() const
{
    return &batch_->sprites()[i_];
}

Sprite::Sprite(unsigned int w, unsigned int h, NotNull<const SpriteBatch*> p)
    : width_(w), height_(h), state_(0), angle_(0.0f), pivot_(0.5f, 0.5f),
      scale_(1.0f, 1.0f), parent_(p), vertex_array_(nullptr),
      normal_map_(nullptr), id_(kNoId)
{
}

Sprite::Sprite(Sprite&& s)
    : width_(s.width_), height_(s.height_),
      state_(s.state_ | kStaleFrontBuffer), angle_(s.angle_), pivot_(s.pivot_),
      center_(s.center_), position_(s.position_), scale_(s.scale_),
      parent_(s.parent_), vertex_array_(s.vertex_array_),
      normal_map_(s.normal_map_), id_(s.id_)
{
    s.parent_ = nullptr;
    s.vertex_array_ = nullptr;
    s.id_ = kNoId;
}

auto Sprite::is_flipped() const -> bool
{
    return (state_ & kIsFlipped) == kIsFlipped;
}

auto Sprite::is_hidden() const -> bool
{
    return (state_ & kIsHidden) == kIsHidden;
}

auto Sprite::is_mirrored() const -> bool
{
    return (state_ & kIsMirrored) == kIsMirrored;
}

void Sprite::set_color(Colorb c)
{
    R_ASSERT(vertex_array_ != nullptr, "Missing vertex array buffer");
    ASSUME(vertex_array_ != nullptr);

    vertex_array_[0].color = c;
    vertex_array_[1].color = c;
    vertex_array_[2].color = c;
    vertex_array_[3].color = c;
    state_ |= kStaleFrontBuffer;
}

void Sprite::set_normal(unsigned int id)
{
    R_ASSERT(normal_map_ != nullptr, "Missing normal map buffer");
    ASSUME(normal_map_ != nullptr);

    set_normal(flip_index(state_), parent_->normal()[id].vx);
    state_ |= kStaleFrontBuffer;
}

void Sprite::set_pivot(const Vec2f& pivot)
{
    R_ASSERT(pivot.x >= 0.0f && pivot.x <= 1.0f &&
             pivot.y >= 0.0f && pivot.y <= 1.0f,
             "Invalid pivot point");

    Vec2f diff = pivot;
    diff -= pivot_;
    if (diff.is_zero())
        return;

    diff.x *= width_ * scale_.x;
    diff.y *= height_ * scale_.y;
    center_ += diff;
    position_ += diff;
    pivot_ = pivot;
}

void Sprite::set_position(const Vec2f& position)
{
    position_ = position;
    state_ |= kStalePosition;
}

void Sprite::set_rotation(float r)
{
    angle_ = r;
    state_ |= kStaleBuffer;
}

void Sprite::set_scale(const Vec2f& f)
{
    R_ASSERT(f.x > 0.0f && f.y > 0.0f,
             "Can't scale with a factor of zero or less");

    scale_ = f;
    state_ |= kStaleBuffer;
}

void Sprite::set_texture(unsigned int id)
{
    R_ASSERT(vertex_array_ != nullptr, "Missing vertex array buffer");
    ASSUME(vertex_array_ != nullptr);

    const unsigned int f = flip_index(state_);
    const auto& tx = parent_->texture()[id];
    vertex_array_[kFlipTable[f + 0]].texcoord = tx.vx[0];
    vertex_array_[kFlipTable[f + 1]].texcoord = tx.vx[1];
    vertex_array_[kFlipTable[f + 2]].texcoord = tx.vx[2];
    vertex_array_[kFlipTable[f + 3]].texcoord = tx.vx[3];

    state_ |= kStaleFrontBuffer;
}

void Sprite::set_vertex_array(NotNull<SpriteVertex*> array)
{
    vertex_array_ = array;
    state_ |= kStaleMask;
}

void Sprite::flip()
{
    state_ ^= kIsFlipped;
    flip_textures(kFlipVertically);
}

void Sprite::hide()
{
    if (is_hidden())
        return;

    vertex_array_[0].position = Vec2f::Zero;
    vertex_array_[1].position = Vec2f::Zero;
    vertex_array_[2].position = Vec2f::Zero;
    vertex_array_[3].position = Vec2f::Zero;
    state_ |= kIsHidden | kStaleFrontBuffer;
}

void Sprite::mirror()
{
    state_ ^= kIsMirrored;
    flip_textures(kFlipHorizontally);
}

void Sprite::move(const Vec2f& delta)
{
    position_ += delta;
    state_ |= kStalePosition;
}

void Sprite::rotate(float r)
{
    angle_ += r;
    state_ |= kStaleBuffer;
}

void Sprite::show()
{
    if (!is_hidden())
        return;

    state_ &= ~kIsHidden;
    state_ |= kStaleMask;
}

auto Sprite::update() -> bool
{
    if ((state_ & kStaleMask) == 0)
        return false;

    if (is_hidden())
    {
        state_ &= ~kStaleMask;
        return true;
    }

    R_ASSERT(vertex_array_ != nullptr, "Missing vertex array buffer");
    ASSUME(vertex_array_ != nullptr);

    if (state_ & kStaleBuffer)
    {
        if (state_ & kStalePosition)
            center_ = position_;

        rainbow::transform(*this, ArraySpan<SpriteVertex>(vertex_array_, 4));
    }
    else if (state_ & kStalePosition)
    {
        position_ -= center_;
        vertex_array_[0].position += position_;
        vertex_array_[1].position += position_;
        vertex_array_[2].position += position_;
        vertex_array_[3].position += position_;
        center_ += position_;
        position_ = center_;
    }
    state_ &= ~kStaleMask;
    return true;
}

Sprite& Sprite::operator=(Sprite&& s)
{
    width_ = s.width_;
    height_ = s.height_;
    state_ = s.state_ | kStaleFrontBuffer;
    angle_ = s.angle_;
    pivot_ = s.pivot_;
    center_ = s.center_;
    position_ = s.position_;
    scale_ = s.scale_;
    parent_ = s.parent_;
    vertex_array_ = s.vertex_array_;
    normal_map_ = s.normal_map_;
    id_ = s.id_;
    s.parent_ = nullptr;
    s.vertex_array_ = nullptr;
    s.id_ = kNoId;
    return *this;
}

void Sprite::flip_textures(unsigned int f)
{
    R_ASSERT(vertex_array_ != nullptr, "Missing vertex array buffer");
    ASSUME(vertex_array_ != nullptr);

    const Vec2f coords[]{vertex_array_[kFlipTable[f + 0]].texcoord,
                         vertex_array_[kFlipTable[f + 1]].texcoord,
                         vertex_array_[kFlipTable[f + 2]].texcoord,
                         vertex_array_[kFlipTable[f + 3]].texcoord};
    vertex_array_[0].texcoord = coords[0];
    vertex_array_[1].texcoord = coords[1];
    vertex_array_[2].texcoord = coords[2];
    vertex_array_[3].texcoord = coords[3];

    if (normal_map_)
    {
        const Vec2f coords[]{
            normal_map_[0], normal_map_[1], normal_map_[2], normal_map_[3]};
        set_normal(f, coords);
    }

    state_ |= kStaleFrontBuffer;
}

void Sprite::set_normal(unsigned int f, NotNull<const Vec2f*> uv)
{
    normal_map_[kFlipTable[f + 0]] = uv[0];
    normal_map_[kFlipTable[f + 1]] = uv[1];
    normal_map_[kFlipTable[f + 2]] = uv[2];
    normal_map_[kFlipTable[f + 3]] = uv[3];
}
