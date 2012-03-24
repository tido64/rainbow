#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Graphics/Drawable.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"

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
	SpriteBatch(const unsigned int hint = 8);
	~SpriteBatch();

	Sprite* add(const int x, const int y, const int width, const int height);

	/// Return all sprites within this batch.
	inline Vector<Sprite*>& get_sprites();

	/// Load texture data.
	TextureAtlas* set_texture(const Data &texture);

	/// Re-use a texture atlas.
	TextureAtlas* set_texture(TextureAtlas *texture);

	/// Re-use a texture atlas.
	SmartPtr<TextureAtlas>& set_texture(SmartPtr<TextureAtlas> &texture);

	/// Return the number of sprites.
	inline unsigned int size();

	/// Draw the batch of sprites.
	virtual void draw();

	/// Update the batch of sprites.
	virtual void update();

protected:
	/// Add a sprite to the batch.
	/// \param s  The sprite to add
	void push_back(Sprite *s);

private:
	unsigned int buffer;             ///< GL vertex buffer.
	unsigned int batch_vertices;     ///< Total number of vertices in this batch.
	unsigned int reserved;           ///< Allocated memory for sprite vertices.
	SpriteVertex *vertex_buffer;     ///< Common vertex array for all sprites in the batch.
	SmartPtr<TextureAtlas> texture;  ///< Texture atlas used by all sprites in the batch.
	Vector<Sprite*> sprites;         ///< Vector storing all sprites.

	/// Intentionally left undefined.
	SpriteBatch(const SpriteBatch &);

	/// Intentionally left undefined.
	SpriteBatch& operator=(const SpriteBatch &);
};

Vector<Sprite*>& SpriteBatch::get_sprites()
{
	return this->sprites;
}

unsigned int SpriteBatch::size()
{
	return this->sprites.size();
}

#endif
