// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Graphics/Label.h"

Label::~Label()
{
	delete[] this->text;
	delete[] this->vx;
}

void Label::set_text(const char *text)
{
	const unsigned int len = strlen(text);
	if (len > this->length)
	{
		delete[] this->text;
		this->text = new char[len + 1];
		this->length = len;
	}
	memcpy(this->text, text, len);
	this->text[len] = '\0';
	this->stale |= stale_position;
}

void Label::draw()
{
	this->font->bind();
	Renderer::draw_elements(this->vx, (this->length << 2) + (this->length << 1));
}

void Label::update()
{
	// Note: This algorithm currently does not support font kerning.
	if (this->stale)
	{
		const unsigned int str_sz = this->length << 2;
		if (this->stale & stale_position)
		{
			if (str_sz > this->size)
			{
				delete[] this->vx;
				this->vx = new SpriteVertex[str_sz];
				this->size = str_sz;
				this->stale |= stale_color;
			}

			SpriteVertex *vx = this->vx;
			Vec2f pen = this->position;
			for (const char *text = this->text; *text; ++text)
			{
				const FontGlyph &glyph = this->font->get_glyph(*text);
				pen.x += glyph.left;

				for (unsigned int i = 0; i < 4; ++i)
				{
					vx->texcoord = glyph.quad[i].texcoord;
					vx->position = glyph.quad[i].position;
					vx->position += pen;
					++vx;
				}

				pen.x += glyph.advance - glyph.left;
			}
		}
		if (this->stale & stale_color)
		{
			for (unsigned int i = 0; i < this->size; ++i)
				this->vx[i].color = this->color;
		}
		this->stale = 0u;
	}
}
