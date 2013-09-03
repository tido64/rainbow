// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Label.h"

namespace
{
	const unsigned int kStaleBuffer      = 1u << 0;
	const unsigned int kStaleBufferSize  = 1u << 1;
	const unsigned int kStaleColor       = 1u << 2;
}

void Label::set_alignment(const Label::Alignment a)
{
	this->alignment = a;
	this->stale |= kStaleBuffer;
}

void Label::set_color(const Colorb &c)
{
	this->color = c;
	this->stale |= kStaleColor;
}

void Label::set_font(FontAtlas *f)
{
	this->font = f;
	this->stale |= kStaleBuffer;
}

void Label::set_position(const Vec2f &position)
{
	this->position.x = static_cast<int>(position.x + 0.5);
	this->position.y = static_cast<int>(position.y + 0.5);
	this->stale |= kStaleBuffer;
}

void Label::set_scale(const float f)
{
	if (Rainbow::equal(this->scale, f))
		return;

	if (f >= 1.0f)
		this->scale = 1.0f;
	else if (f <= 0.01f)
		this->scale = 0.01f;
	else
		this->scale = f;

	this->stale |= kStaleBuffer;
}

void Label::set_text(const char *text)
{
	const size_t len = strlen(text);
	if (len > this->size)
	{
		this->text.reset(new char[len + 1]);
		this->size = len;
		this->stale |= kStaleBufferSize;
	}
	memcpy(this->text.get(), text, len);
	this->text[len] = '\0';
	this->stale |= kStaleBuffer;
}

void Label::move(const Vec2f &delta)
{
	this->position += delta;
	this->stale |= kStaleBuffer;
}

void Label::update()
{
	// Note: This algorithm currently does not support font kerning.
	if (this->stale)
	{
		if (this->stale & kStaleBuffer)
		{
			if (this->stale & kStaleBufferSize)
			{
				this->vx.reset(new SpriteVertex[this->size * 4]);
				this->stale |= kStaleColor;
			}

			this->array.count = 0;
			size_t start = 0;
			SpriteVertex *vx = this->vx.get();
			Vec2f pen = this->position;
			for (const unsigned char *text = reinterpret_cast<unsigned char*>(this->text.get()); *text;)
			{
				if (*text == '\n')
				{
					this->align(this->position.x - pen.x, start, this->array.count);
					pen.x = this->position.x;
					start = this->array.count;
					pen.y -= this->font->get_height() * this->scale;
					++text;
					continue;
				}

				const Rainbow::utf_t &c = Rainbow::utf8_decode(text);
				if (c.bytes == 0)
					break;
				text += c.bytes;

				const FontGlyph *glyph = this->font->get_glyph(c);
				if (!glyph)
					continue;

				pen.x += glyph->left * this->scale;

				for (size_t i = 0; i < 4; ++i)
				{
					vx->texcoord = glyph->quad[i].texcoord;
					vx->position = glyph->quad[i].position;
					vx->position *= this->scale;
					vx->position += pen;
					++vx;
				}

				pen.x += (glyph->advance - glyph->left) * this->scale;
				++this->array.count;
			}
			this->align(this->position.x - pen.x, start, this->array.count);
			this->width = pen.x - this->position.x;
			this->array.count = this->array.count * 4 + this->array.count * 2;
		}
		if (this->stale & kStaleColor)
		{
			for (size_t i = 0; i < this->size * 4; ++i)
				this->vx[i].color = this->color;
		}
		this->array.update(this->vx.get(), this->size * 4 * sizeof(SpriteVertex));
		this->stale = 0;
	}
}

void Label::align(float offset, size_t start, size_t end)
{
	if (this->alignment != kLeftTextAlignment)
	{
		if (this->alignment == kCenterTextAlignment)
			offset *= 0.5f;

		start <<= 2;
		end <<= 2;
		for (size_t i = start; i < end; ++i)
			this->vx[i].position.x += offset;
	}
}
