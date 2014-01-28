// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Label.h"

namespace
{
	const unsigned int kStaleBuffer      = 1u << 0;
	const unsigned int kStaleBufferSize  = 1u << 1;
	const unsigned int kStaleColor       = 1u << 2;
}

Label::Label() :
	scale_(1.0f), alignment_(kLeftTextAlignment), count_(0), stale_(0),
	width_(0), size_(0),
	array_([this]() {
		this->font_->bind();
		this->buffer_.bind();
	}) { }

void Label::set_alignment(const Label::Alignment a)
{
	this->alignment_ = a;
	this->stale_ |= kStaleBuffer;
}

void Label::set_color(const Colorb &c)
{
	this->color_ = c;
	this->stale_ |= kStaleColor;
}

void Label::set_font(FontAtlas *f)
{
	this->font_ = f;
	this->stale_ |= kStaleBuffer;
}

void Label::set_position(const Vec2f &position)
{
	this->position_.x = static_cast<int>(position.x + 0.5);
	this->position_.y = static_cast<int>(position.y + 0.5);
	this->stale_ |= kStaleBuffer;
}

void Label::set_scale(const float f)
{
	if (Rainbow::equal(this->scale_, f))
		return;

	if (f >= 1.0f)
		this->scale_ = 1.0f;
	else if (f <= 0.01f)
		this->scale_ = 0.01f;
	else
		this->scale_ = f;

	this->stale_ |= kStaleBuffer;
}

void Label::set_text(const char *text)
{
	const size_t len = strlen(text);
	if (len > this->size_)
	{
		this->text_.reset(new char[len + 1]);
		this->size_ = len;
		this->stale_ |= kStaleBufferSize;
	}
	memcpy(this->text_.get(), text, len);
	this->text_[len] = '\0';
	this->stale_ |= kStaleBuffer;
}

void Label::move(const Vec2f &delta)
{
	this->position_ += delta;
	this->stale_ |= kStaleBuffer;
}

void Label::update()
{
	// Note: This algorithm currently does not support font kerning.
	if (this->stale_)
	{
		if (this->stale_ & kStaleBuffer)
		{
			Vector<SpriteVertex> &buffer = this->buffer_.storage();
			if (this->stale_ & kStaleBufferSize)
			{
				buffer.reserve(this->size_ * 4);
				this->stale_ |= kStaleColor;
			}
			size_t count = 0;
			size_t start = 0;
			SpriteVertex *vx = buffer.begin();
			Vec2f pen = this->position_;
			for (const unsigned char *text =
			         reinterpret_cast<unsigned char*>(this->text_.get());
			     *text;)
			{
				if (*text == '\n')
				{
					this->align(this->position_.x - pen.x, start, count);
					pen.x = this->position_.x;
					start = count;
					pen.y -= this->font_->height() * this->scale_;
					++text;
					continue;
				}

				const Rainbow::utf_t &c = Rainbow::utf8_decode(text);
				if (c.bytes == 0)
					break;
				text += c.bytes;

				const FontGlyph *glyph = this->font_->get_glyph(c);
				if (!glyph)
					continue;

				pen.x += glyph->left * this->scale_;

				for (size_t i = 0; i < 4; ++i)
				{
					vx->texcoord = glyph->quad[i].texcoord;
					vx->position = glyph->quad[i].position;
					vx->position *= this->scale_;
					vx->position += pen;
					++vx;
				}

				pen.x += (glyph->advance - glyph->left) * this->scale_;
				++count;
			}
			this->count_ = count * 4;
			this->align(this->position_.x - pen.x, start, count);
			this->width_ = pen.x - this->position_.x;
		}
		if (this->stale_ & kStaleColor)
		{
			SpriteVertex *buffer = this->buffer_.storage().begin();
			for (size_t i = 0; i < this->count_; ++i)
				buffer[i].color = this->color_;
		}
		this->buffer_.upload(this->count_);
		this->stale_ = 0;
	}
}

void Label::align(float offset, size_t start, size_t end)
{
	if (this->alignment_ != kLeftTextAlignment)
	{
		if (this->alignment_ == kCenterTextAlignment)
			offset *= 0.5f;

		start <<= 2;
		end <<= 2;
		SpriteVertex *buffer = this->buffer_.storage().begin();
		for (size_t i = start; i < end; ++i)
			buffer[i].position.x += offset;
	}
}
