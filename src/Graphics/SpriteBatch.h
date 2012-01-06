#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Common/Vector.h"
#include "Graphics/Drawable.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Sprite.h"
#include "Graphics/Texture.h"

/// A drawable batch of sprites.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class SpriteBatch : public Drawable
{
	friend void Sprite::set_texture(const unsigned int id);

public:
	SpriteBatch(const int hint = 8);
	~SpriteBatch();

	Sprite* add(const int x, const int y, const int width, const int height);

	/// Draw the batch of sprites.
	virtual void draw();

	/// Return all sprites within this batch.
	Vector<Sprite*>& get_sprites();

	/// Load texture data.
	Texture* set_texture(const Data &texture);

	/// Re-use a texture.
	Texture* set_texture(Texture *texture);

	/// Re-use a texture.
	SmartPtr<Texture>& set_texture(SmartPtr<Texture> &texture);

	/// Return the number of sprites.
	unsigned int size();

	/// Update the batch of sprites.
	virtual void update();

protected:
	static const unsigned char indices[];  ///< Global vertex indices (currently limited to 64 vertices, or 16 sprites)

	unsigned int align(const unsigned int i);

	/// Add a sprite to the batch.
	/// \param s  The sprite to add
	void push_back(Sprite *s);

private:
	GLuint buffer;                 ///< GL vertex buffer.
	unsigned int batch_vertices;   ///< Total number of vertices in this batch.
	unsigned int reserved;         ///< Allocated memory for sprite vertices.
	SpriteVertex *vertex_buffer;   ///< Common vertex array for all sprites in the batch.
	SmartPtr<Texture> texture;     ///< Texture atlas used by all sprites in the batch.
	Vector<Sprite*> sprites;       ///< Vector storing all sprites.

	/// Intentionally left undefined.
	SpriteBatch(const SpriteBatch &);

	/// Intentionally left undefined.
	SpriteBatch& operator=(const SpriteBatch &);
};

inline unsigned int SpriteBatch::align(const unsigned int i)
{
	return (i << 2) + (i << 1);
}

inline Vector<Sprite*>& SpriteBatch::get_sprites()
{
	return this->sprites;
}

inline unsigned int SpriteBatch::size()
{
	return this->sprites.size();
}

#endif
