#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Graphics/Drawable.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"

/// A drawable batch of sprites.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class SpriteBatch : public Drawable
{
	friend void Sprite::set_texture(const unsigned int id);

public:
	SpriteBatch(const int hint = 8);
	~SpriteBatch() { delete[] this->vertex_buffer; }

	Sprite* add(const int x, const int y, const int width, const int height);

	/// Draw the batch of sprites.
	void draw();

	/// Load texture data.
	Texture* set_texture(const void *const texture);

	/// Re-use specified texture.
	Texture* set_texture(Texture *texture);

	/// Return the number of sprites.
	unsigned int size();

	/// Update the batch of sprites.
	void update();

protected:
	static const void *tx_offset;
	static const void *vx_offset;

	unsigned int align(const unsigned int i);

	/// Add a sprite to the batch.
	/// \param s  The sprite to add
	void push_back(Sprite *s);

private:
	GLuint buffer;                 ///< GL vertex buffer
	unsigned int batch_buffer_sz;  ///< Size of batch buffer
	unsigned int batch_vertices;   ///< Total number of vertices in this batch
	unsigned int reserved;         ///< Allocated memory for sprite vertices
	SpriteVertex *vertex_buffer;   ///< Common vertex array for all sprites in the batch
	Texture *texture;              ///< Texture
	Vector<Sprite *> sprites;      ///< Vector storing all sprites

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
