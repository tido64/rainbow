// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

#include "Director.h"
#include "Math/Transform.h"
#include "Text/Typesetter.h"

using rainbow::Color;
using rainbow::czstring;
using rainbow::Label;
using rainbow::TextAlignment;
using rainbow::Vec2f;

Label::Label(graphics::Driver&)
    : stale_(0), font_size_(15), alignment_(TextAlignment::Left), scale_(1.0f),
      angle_(0.0f)
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

void Label::set_alignment(TextAlignment a)
{
    alignment_ = a;
    set_needs_update(kStaleBuffer);
}

void Label::set_color(Color c)
{
    color_ = c;
    set_needs_update(kStaleColor);
}

void Label::set_font(czstring font_face)
{
    font_face_ = font_face == nullptr ? "" : font_face;
    set_needs_update(kStaleBuffer);
}

void Label::set_font_size(int font_size)
{
    font_size_ = font_size;
    set_needs_update(kStaleBuffer);
}

void Label::set_position(const Vec2f& position)
{
    position_.x = std::round(position.x);
    position_.y = std::round(position.y);
    set_needs_update(kStaleBuffer);
}

void Label::set_rotation(float r)
{
    if (are_equal(r, angle_))
        return;

    angle_ = r;
    set_needs_update(kStaleBuffer);
}

void Label::set_scale(float f)
{
    if (are_equal(f, scale_))
        return;

    scale_ = std::clamp(f, 0.01f, 1.0f);
    set_needs_update(kStaleBuffer);
}

void Label::set_text(czstring text)
{
    text_ = text;
    set_needs_update(kStaleBuffer);
}

void Label::move(const Vec2f& delta)
{
    position_ += delta;
    set_needs_update(kStaleBuffer);
}

void Label::update()
{
    if (stale_ != 0)
    {
        update_internal();
        upload();
        clear_state();
    }
}

void Label::update_internal()
{
    if ((stale_ & kStaleBuffer) != 0)
    {
        vertices_ = Typesetter::Get()->draw_text(
            text_,
            position_,
            TextAttributes{font_face_, font_size_, alignment_},
            &size_);
        for (auto&& vx : vertices_)
            vx.color = color_;
    }
    else if ((stale_ & kStaleColor) != 0)
    {
        for (auto&& vx : vertices_)
            vx.color = color_;
    }
}

void Label::upload() const
{
    buffer_.upload(vertices_.data(), vertices_.size() * sizeof(vertices_[0]));
}
