// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Algorithm.h"
#include "Graphics/Label.h"

Label::~Label()
{
	delete[] this->text;
	delete[] this->vx;
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
	this->stale |= stale_position;
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
		if (this->stale & stale_position)
		{
			if (this->stale & stale_vbo)
			{
				delete[] this->vx;
				this->vx = new SpriteVertex[this->size << 2];
				this->stale |= stale_color;
			}

			this->vertices = 0;
			SpriteVertex *vx = this->vx;
			Vec2f pen = this->position;
			for (const unsigned char *text = reinterpret_cast<unsigned char*>(this->text); *text;)
			{
				size_t bytes;
				const unsigned long c = Rainbow::utf8_decode(text, bytes);
				if (!bytes)
					break;
				text += bytes;

				const FontGlyph *glyph = this->font->get_glyph(c);
				if (!glyph)
					continue;

				pen.x += glyph->left;

				for (size_t i = 0; i < 4; ++i)
				{
					vx->texcoord = glyph->quad[i].texcoord;
					vx->position = glyph->quad[i].position;
					vx->position += pen;
					++vx;
				}

				pen.x += glyph->advance - glyph->left;
				++this->vertices;
			}
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
