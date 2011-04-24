/// A batch of sprites to be drawn.

/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Sprite.h"

class SpriteBatch
{
public:
	SpriteBatch(const int hint = 8) :
		texture(0), count(0), reserved(indexof(hint)), sprites(hint)
	{
		this->vertex_buffer = new SpriteVertex[this->reserved];
		glGenBuffers(1, &this->buffer);
	}

	~SpriteBatch() { delete[] this->vertex_buffer; }

	/// Add a sprite to the batch.
	/// \param s  The sprite to add
	void add(Sprite *s);

	/// Draw the batch of sprites.
	void draw();

	/// Update the batch of sprites.
	void update();

protected:
	unsigned int indexof(const unsigned int i);

private:
	GLuint buffer;
	GLuint texture;
	unsigned int batch_buffer_sz;
	unsigned int batch_vertices;
	unsigned int count;
	unsigned int reserved;
	Vector<Sprite *> sprites;
	SpriteVertex *vertex_buffer;

	SpriteBatch(const SpriteBatch &sb);
	SpriteBatch& operator=(const SpriteBatch &);
};

inline unsigned int SpriteBatch::indexof(const unsigned int i)
{
	return (i << 2) + (i << 1);
}

#endif
