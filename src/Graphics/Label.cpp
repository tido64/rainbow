// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

#include <algorithm>

namespace
{
	const unsigned int kStaleBuffer      = 1u << 0;
	const unsigned int kStaleBufferSize  = 1u << 1;
	const unsigned int kStaleColor       = 1u << 2;
}

Label::Label()
    : size_(0), scale_(1.0f), alignment_(kLeftTextAlignment), count_(0),
      stale_(0), width_(0)
{
	array_.reconfigure([=] { buffer_.bind(); });
}

void Label::set_alignment(const Label::Alignment a)
{
	alignment_ = a;
	stale_ |= kStaleBuffer;
}

void Label::set_color(const Colorb &c)
{
	color_ = c;
	stale_ |= kStaleColor;
}

void Label::set_font(FontAtlas *f)
{
	font_ = f;
	stale_ |= kStaleBuffer;
}

void Label::set_position(const Vec2f &position)
{
	position_.x = static_cast<int>(position.x + 0.5);
	position_.y = static_cast<int>(position.y + 0.5);
	stale_ |= kStaleBuffer;
}

void Label::set_scale(const float f)
{
	if (Rainbow::isequal(scale_, f))
		return;

	if (f >= 1.0f)
		scale_ = 1.0f;
	else if (f <= 0.01f)
		scale_ = 0.01f;
	else
		scale_ = f;

	stale_ |= kStaleBuffer;
}

void Label::set_text(const char *text)
{
	const size_t len = strlen(text);
	if (len > size_)
	{
		text_.reset(new char[len + 1]);
		size_ = len;
		stale_ |= kStaleBufferSize;
	}
	memcpy(text_.get(), text, len);
	text_[len] = '\0';
	stale_ |= kStaleBuffer;
}

void Label::bind_textures() const
{
	font_->bind();
}

void Label::move(const Vec2f &delta)
{
	position_ += delta;
	stale_ |= kStaleBuffer;
}

void Label::update()
{
	// Note: This algorithm currently does not support font kerning.
	if (stale_)
	{
		if (stale_ & kStaleBuffer)
		{
			if (stale_ & kStaleBufferSize)
			{
				vertices_.reset(new SpriteVertex[size_ * 4]);
				stale_ |= kStaleColor;
			}
			size_t count = 0;
			size_t start = 0;
			SpriteVertex *vx = vertices_.get();
			Vec2f pen = position_;
			for (const unsigned char *text =
			         reinterpret_cast<unsigned char*>(text_.get());
			     *text;)
			{
				if (*text == '\n')
				{
					align(position_.x - pen.x, start, count);
					pen.x = position_.x;
					start = count;
					pen.y -= font_->height() * scale_;
					++text;
					continue;
				}

				const Rainbow::utf_t &c = Rainbow::utf8_decode(text);
				if (c.bytes == 0)
					break;
				text += c.bytes;

				const FontGlyph *glyph = font_->get_glyph(c);
				if (!glyph)
					continue;

				pen.x += glyph->left * scale_;

				for (size_t i = 0; i < 4; ++i)
				{
					vx->texcoord = glyph->quad[i].texcoord;
					vx->position = glyph->quad[i].position;
					vx->position *= scale_;
					vx->position += pen;
					++vx;
				}

				pen.x += (glyph->advance - glyph->left) * scale_;
				++count;
			}
			count_ = count * 4;
			align(position_.x - pen.x, start, count);
			width_ = pen.x - position_.x;
		}
		if (stale_ & kStaleColor)
		{
			const Colorb &color = color_;
			std::for_each(vertices_.get(),
			              vertices_.get() + count_,
			              [color](SpriteVertex &v) {
				v.color = color;
			});
		}
		buffer_.upload(vertices_.get(), count_ * sizeof(SpriteVertex));
		stale_ = 0;
	}
}

void Label::align(float offset, const size_t start, const size_t end)
{
	if (alignment_ != kLeftTextAlignment)
	{
		if (alignment_ == kCenterTextAlignment)
			offset *= 0.5f;
		std::for_each(vertices_.get() + start * 4,
		              vertices_.get() + end * 4,
		              [offset](SpriteVertex &v) {
			v.position.x += offset;
		});
	}
}
