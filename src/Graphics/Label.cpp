// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

#include "Graphics/Driver.h"
#include "Text/Typesetter.h"

using rainbow::Color;
using rainbow::Label;
using rainbow::TextAlignment;
using rainbow::Vec2f;
using rainbow::graphics::Driver;

Label::Label(const Driver& driver)
    : stale_(0), buffer_(driver.make_buffer<vk::DynamicVertexBuffer>()),
      font_size_(15), alignment_(TextAlignment::Left), scale_(1.0f),
      angle_(0.0f)
{
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

void Label::set_font(std::string font_face)
{
    font_face_ = std::move(font_face);
    set_needs_update(kStaleBuffer);
}

void Label::set_font_size(int font_size)
{
    font_size_ = font_size;
    set_needs_update(kStaleBuffer);
}

void Label::set_position(Vec2f position)
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

void Label::set_text(std::string text)
{
    text_ = std::move(text);
    set_needs_update(kStaleBuffer);
}

void Label::move(Vec2f delta)
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

void Label::upload()
{
    buffer_.copy(vertices_);
}

void rainbow::vk::draw(const CommandBuffer& command_buffer,
                       const Label& label,
                       const IndexBuffer& index_buffer)
{
    const auto vertex_count = label.vertex_count();
    if (vertex_count == 0)
        return;

    update_descriptor(command_buffer, FontCache::Get()->texture());
    draw(command_buffer, label.vertex_buffer(), vertex_count, index_buffer);
}
