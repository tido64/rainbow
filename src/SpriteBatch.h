#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Drawable.h"
#include "Sprite.h"

/// A batch of sprites to be drawn.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
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

	/// Return the number of sprites.
	unsigned int size();

	/// Update the batch of sprites.
	void update();

protected:
	unsigned int align(const unsigned int i);

private:
	GLuint buffer;                 ///< GL vertex buffer
	GLuint texture;                ///< GL texture name
	unsigned int batch_buffer_sz;  ///< Size of batch buffer
	unsigned int batch_vertices;   ///< Total number of vertices in this batch
	unsigned int reserved;         ///< Allocated memory for sprite vertices
	Vector<Sprite *> sprites;      ///< Vector storing all sprites
	SpriteVertex *vertex_buffer;   ///< Common vertex array for all sprites in the batch

	/// Intentionally left undefined.
	SpriteBatch(const SpriteBatch &);

	/// Intentionally left undefined.
	SpriteBatch& operator=(const SpriteBatch &);
};

inline unsigned int SpriteBatch::align(const unsigned int i)
{
	return (i << 2) + (i << 1);
}

inline unsigned int SpriteBatch::size()
{
	return this->sprites.size();
}

#endif
