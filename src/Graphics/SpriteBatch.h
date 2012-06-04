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
	SpriteBatch(const size_t hint = 8);
	virtual ~SpriteBatch();

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
	inline size_t size();

	/// Draw the batch of sprites.
	virtual void draw() override;

	/// Update the batch of sprites.
	virtual void update() override;

protected:
	/// Add a sprite to the batch.
	/// \param s  The sprite to add
	void push_back(Sprite *s);

private:
	unsigned int array_object;       ///< GL vertex array object.
	unsigned int buffer;             ///< GL vertex buffer.

	size_t batch_vertices;           ///< Total number of vertices in this batch.
	size_t reserved;                 ///< Allocated memory for sprite vertices.
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

size_t SpriteBatch::size()
{
	return this->sprites.size();
}

#endif
