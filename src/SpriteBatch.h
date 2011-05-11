/// A batch of sprites to be drawn.

/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Drawable.h"
#include "Sprite.h"

class SpriteBatch : public Drawable
{
public:
	SpriteBatch(const int hint = 8);
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

	/// Intentionally left undefined.
	SpriteBatch(const SpriteBatch &);

	/// Intentionally left undefined.
	SpriteBatch& operator=(const SpriteBatch &);
};

inline unsigned int SpriteBatch::indexof(const unsigned int i)
{
	return (i << 2) + (i << 1);
}

#endif
