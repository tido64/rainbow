// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

#include "Math/Transform.h"
#include "Script/GameBase.h"

using rainbow::Color;
using rainbow::czstring;
using rainbow::GameBase;
using rainbow::Label;
using rainbow::TextAlignment;
using rainbow::Vec2f;

Label::Label()
{
    array_.reconfigure([this] { buffer_.bind(); });
}

Label::~Label()
{
#ifndef NDEBUG
    Director::assert_unused(
        this, "Label deleted but is still in the render queue.");
#endif
}

auto Label::alignment(TextAlignment a) -> Label&
{
    alignment_ = a;
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::angle(float r) -> Label&
{
    if (are_equal(r, angle_)) {
        return *this;
    }

    angle_ = r;
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::color(Color c) -> Label&
{
    color_ = c;
    set_needs_update(kStaleColor);
    return *this;
}

auto Label::font(czstring font_face) -> Label&
{
    font_face_ = font_face == nullptr ? "" : font_face;
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::font_size(int font_size) -> Label&
{
    font_size_ = font_size;
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::move(Vec2f delta) -> Label&
{
    position_ += delta;
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::position(Vec2f position) -> Label&
{
    position_.x = std::round(position.x);
    position_.y = std::round(position.y);
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::scale(float f) -> Label&
{
    if (are_equal(f, scale_)) {
        return *this;
    }

    constexpr auto min_scale = 0.01F;
    scale_ = std::clamp(f, min_scale, 1.0F);
    set_needs_update(kStaleBuffer);
    return *this;
}

auto Label::text(czstring text) -> Label&
{
    text_ = text;
    set_needs_update(kStaleBuffer);
    return *this;
}

void Label::update(GameBase& context)
{
    if (stale_ != 0) {
        update_internal(context);
        upload();
        clear_state();
    }
}

void Label::update_internal(GameBase& context)
{
    if ((stale_ & kStaleBuffer) != 0) {
        vertices_ = context.typesetter().draw_text(
            text_,
            position_,
            TextAttributes{font_face_, font_size_, alignment_},
            &size_);
        for (auto&& vx : vertices_) {
            vx.color = color_;
        }
    } else if ((stale_ & kStaleColor) != 0) {
        for (auto&& vx : vertices_) {
            vx.color = color_;
        }
    }
}

void Label::upload() const
{
    buffer_.upload(vertices_.data(), vertices_.size() * sizeof(vertices_[0]));
}

void rainbow::graphics::draw(Context& ctx, const Label& label)
{
    auto& font_cache = *FontCache::Get();
    bind(ctx, font_cache.texture());
    draw(label.vertex_array(), label.vertex_count());
}
