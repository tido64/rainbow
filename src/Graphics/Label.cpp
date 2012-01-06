/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#include "Graphics/Label.h"

Label::Label(const char *s) :
	length(0), size(0), text(nullptr), vx(nullptr)
{
	this->set_text(s);
}

Label::~Label()
{
	delete[] this->text;
	delete[] this->vx;
}

void Label::draw()
{
	glDisableClientState(GL_COLOR_ARRAY);
	glColor4ub(this->color.r, this->color.g, this->color.b, this->color.a);
	this->font->bind();

	if (this->vx)
	{
		glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->vx->texcoord);
		glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &this->vx->position);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, this->length << 2);
	}
	else
	{
		glPushMatrix();
		glTranslatef(static_cast<GLfloat>(this->position.x), static_cast<GLfloat>(this->position.y), 0.0f);

		#ifdef FONTATLAS_KERNING
			unsigned int prev = 0;  // 'space' has no kerning
		#endif
		for (const char *text = this->text; *text; ++text)
		{
			const FontGlyph &glyph = this->font->get_glyph(*text);

			#ifdef FONTATLAS_KERNING
				glTranslatef(static_cast<float>(glyph.left + glyph.kern[prev]), 0.0f, 0.0f);
				prev = static_cast<int>(*text) - FontAtlas::ascii_offset;
			#else
				glTranslatef(static_cast<float>(glyph.left), 0.0f, 0.0f);
			#endif
			glTexCoordPointer(2, GL_FLOAT, sizeof(SpriteVertex), &glyph.quad[0].texcoord);
			glVertexPointer(2, GL_FLOAT, sizeof(SpriteVertex), &glyph.quad[0].position);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glTranslatef(static_cast<float>(glyph.advance - glyph.left), 0.0f, 0.0f);
		}

		glPopMatrix();
	}

	glColor4ub(0xff, 0xff, 0xff, 0xff);
	glEnableClientState(GL_COLOR_ARRAY);
}

void Label::finalize()
{
	// Note #1: This algorithm does not create degenerate vertices.
	// Note #2: This algorithm currently does not support font kerning.

	const unsigned int str_sz = this->length << 2;
	if (str_sz > this->size)
	{
		delete[] this->vx;
		this->vx = new SpriteVertex[str_sz];
		this->size = str_sz;
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
}
