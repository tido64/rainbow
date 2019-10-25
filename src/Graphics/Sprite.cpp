// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Sprite.h"

#include "Graphics/SpriteBatch.h"
#include "Graphics/Texture.h"
#include "Math/Transform.h"

// TODO: As of iOS 13.2, `using rainbow::Rect` clashes with another definition
// in `MacTypes.h`.
using rainbow::Color;
using rainbow::Sprite;
using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::Vec2f;
using rainbow::graphics::TextureData;

namespace
{
    // clang-format off
    constexpr uint32_t kStaleBuffer     = 1U << 0;
    constexpr uint32_t kStalePosition   = 1U << 1;
    constexpr uint32_t kStaleTexture    = 1U << 2;
    constexpr uint32_t kStaleNormalMap  = 1U << 3;
    constexpr uint32_t kStaleMask       = 0xffffU;
    constexpr uint32_t kIsHidden        = 1U << 16;
    constexpr uint32_t kIsFlipped       = 1U << 17;
    constexpr uint32_t kIsMirrored      = 1U << 18;

    constexpr std::array<uint32_t, 16> kFlipTable{
        0, 1, 2, 3,  // Normal
        3, 2, 1, 0,  // Flipped
        1, 0, 3, 2,  // Mirrored
        2, 3, 0, 1,  // Flipped + mirrored
    };
    // clang-format on

    constexpr uint32_t kFlipVertically = 4;
    constexpr uint32_t kFlipHorizontally = 8;

    static_assert((kIsFlipped >> 0xf) == kFlipVertically);
    static_assert((kIsMirrored >> 0xf) == kFlipHorizontally);

    auto flip_index(uint32_t state) -> uint32_t
    {
        return ((state & kIsFlipped) >> 0xf) + ((state & kIsMirrored) >> 0xf);
    }

    auto normalized_coordinates(const TextureData& texture,
                                const rainbow::Rect& rect)
    {
        const auto left = rect.left / texture.width;
        const auto bottom = rect.bottom / texture.height;
        const auto right = (rect.left + rect.width) / texture.width;
        const auto top = (rect.bottom + rect.height) / texture.height;
        return std::array<Vec2f, 4>{
            Vec2f{left, top},
            Vec2f{right, top},
            Vec2f{right, bottom},
            Vec2f{left, bottom},
        };
    }
}  // namespace

auto SpriteRef::get() const -> Sprite&
{
    return (*batch_)[i_];
}

Sprite::Sprite(Sprite&& s) noexcept
    : state_(s.state_ | kStaleMask), center_(s.center_), position_(s.position_),
      texture_area_(s.texture_area_), color_(s.color_), width_(s.width_),
      height_(s.height_), angle_(s.angle_), pivot_(s.pivot_), scale_(s.scale_),
      normal_map_(s.normal_map_), id_(s.id_)
{
    s.id_ = kNoId;
}

auto Sprite::angle(float r) -> Sprite&
{
    state_ |= kStaleBuffer;
    angle_ = r;
    return *this;
}

auto Sprite::color(Color c) -> Sprite&
{
    state_ |= kStaleTexture;
    color_ = c;
    return *this;
}

auto Sprite::flip() -> Sprite&
{
    state_ ^= kIsFlipped;
    state_ |= kStaleTexture;
    return *this;
}

auto Sprite::hide() -> Sprite&
{
    if (is_hidden())
        return *this;

    state_ |= kIsHidden | kStaleMask;
    return *this;
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

auto Sprite::mirror() -> Sprite&
{
    state_ ^= kIsMirrored;
    state_ |= kStaleTexture;
    return *this;
}

auto Sprite::move(Vec2f delta) -> Sprite&
{
    state_ |= kStalePosition;
    position_ += delta;
    return *this;
}

auto Sprite::normal(const rainbow::Rect& area) -> Sprite&
{
    state_ |= kStaleNormalMap;
    normal_map_ = area;
    return *this;
}

auto Sprite::pivot(Vec2f pivot) -> Sprite&
{
    R_ASSERT(pivot.x >= 0.0F && pivot.x <= 1.0F &&  //
                 pivot.y >= 0.0F && pivot.y <= 1.0F,
             "Invalid pivot point");

    Vec2f diff = pivot - pivot_;
    if (!diff.is_zero())
    {
        diff.x *= width_ * scale_.x;
        diff.y *= height_ * scale_.y;
        center_ += diff;
        position_ += diff;
        pivot_ = pivot;
    }
    return *this;
}

auto Sprite::position(Vec2f position) -> Sprite&
{
    state_ |= kStalePosition;
    position_ = position;
    return *this;
}

auto Sprite::rotate(float r) -> Sprite&
{
    state_ |= kStaleBuffer;
    angle_ += r;
    return *this;
}

auto Sprite::scale(Vec2f f) -> Sprite&
{
    R_ASSERT(f.x > 0.0F && f.y > 0.0F,  //
             "Can't scale with a factor of zero or less");

    state_ |= kStaleBuffer;
    scale_ = f;
    return *this;
}

auto Sprite::show() -> Sprite&
{
    if (!is_hidden())
        return *this;

    state_ &= ~kIsHidden;
    state_ |= kStaleMask;
    return *this;
}

auto Sprite::texture(const rainbow::Rect& area) -> Sprite&
{
    state_ |= kStaleTexture;
    texture_area_ = area;
    return *this;
}

auto Sprite::update(ArraySpan<SpriteVertex> vertex_array,
                    const TextureData& texture) -> bool
{
    if ((state_ & kStaleMask) == 0)
        return false;

    if (is_hidden())
    {
        vertex_array[0].position = Vec2f::Zero;
        vertex_array[1].position = Vec2f::Zero;
        vertex_array[2].position = Vec2f::Zero;
        vertex_array[3].position = Vec2f::Zero;
        state_ &= ~kStaleMask;
        return true;
    }

    if ((state_ & kStaleBuffer) != 0)
    {
        if ((state_ & kStalePosition) != 0)
            center_ = position_;

        rainbow::transform(*this, vertex_array);
    }
    else if ((state_ & kStalePosition) != 0)
    {
        position_ -= center_;
        vertex_array[0].position += position_;
        vertex_array[1].position += position_;
        vertex_array[2].position += position_;
        vertex_array[3].position += position_;
        center_ += position_;
        position_ = center_;
    }

    if ((state_ & kStaleTexture) != 0)
    {
        auto coords = normalized_coordinates(texture, texture_area_);
        const uint32_t f = flip_index(state_);
        for (uint32_t i = 0; i < 4; ++i)
        {
            auto& vx = vertex_array[kFlipTable[f + i]];
            vx.color = color_;
            vx.texcoord = coords[i];
        }
    }

    state_ &= ~kStaleMask;
    return true;
}

auto Sprite::update(ArraySpan<Vec2f> normal_array, const TextureData& normal)
    -> bool
{
    if ((state_ & kStaleNormalMap) == 0)
        return false;

    state_ ^= kStaleNormalMap;

    auto coords = normalized_coordinates(normal, normal_map_);
    const uint32_t f = flip_index(state_);
    for (uint32_t i = 0; i < 4; ++i)
        normal_array[kFlipTable[f + i]] = coords[i];
    return true;
}

auto Sprite::operator=(Sprite&& s) noexcept -> Sprite&
{
    state_ = s.state_ | kStaleMask;
    center_ = s.center_;
    position_ = s.position_;
    texture_area_ = s.texture_area_;
    color_ = s.color_;
    width_ = s.width_;
    height_ = s.height_;
    angle_ = s.angle_;
    pivot_ = s.pivot_;
    scale_ = s.scale_;
    normal_map_ = s.normal_map_;
    id_ = s.id_;

    s.id_ = kNoId;

    return *this;
}
