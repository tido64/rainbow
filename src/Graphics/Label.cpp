// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Common/Algorithm.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"

Label::~Label()
{
	delete[] this->text;
	delete[] this->vx;
}

void Label::set_scale(const float f)
{
	if (Rainbow::equalf(this->scale, f))
		return;

	if (f >= 1.0f)
		this->scale = 1.0f;
	else if (f <= 0.01f)
		this->scale = 0.01f;
	else
		this->scale = f;

	this->stale |= stale_buffer;
}

void Label::set_text(const char *text)
{
	const size_t len = strlen(text);
	if (len > this->size)
	{
		delete[] this->text;
		this->text = new char[len + 1];
		this->size = len;
		this->stale |= stale_vbo;
	}
	memcpy(this->text, text, len);
	this->text[len] = '\0';
	this->stale |= stale_buffer;
}

void Label::draw()
{
	this->font->bind();
	Renderer::draw_elements(this->vx, this->vertices);
}

void Label::update()
{
	// Note: This algorithm currently does not support font kerning.
	if (this->stale)
	{
		if (this->stale & stale_buffer)
		{
			if (this->stale & stale_vbo)
			{
				delete[] this->vx;
				this->vx = new SpriteVertex[this->size << 2];
				this->stale |= stale_color;
			}

			this->vertices = 0;
			size_t start = 0;
			SpriteVertex *vx = this->vx;
			Vec2f pen = this->position;
			for (const unsigned char *text = reinterpret_cast<unsigned char*>(this->text); *text;)
			{
				if (*text == '\n')
				{
					this->align(this->position.x - pen.x, start, this->vertices);
					pen.x = this->position.x;
					start = this->vertices;
					pen.y -= this->font->get_height() * this->scale;
					++text;
					continue;
				}

				size_t bytes;
				const unsigned long c = Rainbow::utf8_decode(text, bytes);
				if (!bytes)
					break;
				text += bytes;

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
				++this->vertices;
			}
			this->align(this->position.x - pen.x, start, this->vertices);
			this->width = pen.x - this->position.x;
			this->vertices = (this->vertices << 2) + (this->vertices << 1);
		}
		if (this->stale & stale_color)
		{
			for (size_t i = 0; i < (this->size << 2); ++i)
				this->vx[i].color = this->color;
		}
		this->stale = 0u;
	}
}

void Label::align(float offset, size_t start, size_t end)
{
	if (this->alignment != LEFT)
	{
		if (this->alignment == CENTER)
			offset *= 0.5f;

		start <<= 2;
		end <<= 2;
		for (size_t i = start; i < end; ++i)
			this->vx[i].position.x += offset;
	}
}
