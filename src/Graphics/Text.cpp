/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Graphics/Text.h"

void Text::draw()
{
	glDisableClientState(GL_COLOR_ARRAY);
	glColor4ub(this->color.r, this->color.g, this->color.b, this->color.a);

	this->font->bind();
	glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->vx->texcoord);
	glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->vx->position);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, this->length);

	glColor4ub(0xff, 0xff, 0xff, 0xff);
	glEnableClientState(GL_COLOR_ARRAY);
}

// Note #1: This algorithm does not create degenerate vertices.
// Note #2: This algorithm currently does not support font kerning.
void Text::set_text(const char *text)
{
	this->length = strlen(text) * 4;

	if (this->length > this->size)
	{
		delete[] this->vx;
		this->vx = new SpriteVertex[this->length];
		this->size = this->length;
	}

	SpriteVertex *vx = this->vx;
	Vec2f pen = this->position;
	for (; *text; ++text)
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
