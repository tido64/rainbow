#ifndef SPRITEBATCH_H_
#define SPRITEBATCH_H_

#include "Graphics/Drawable.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"

namespace SceneGraph { class Node; }

/// A drawable batch of sprites.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class SpriteBatch
{
	friend class SceneGraph::Node;
	friend void Sprite::set_texture(const unsigned int id);

public:
	/// Create a batch of sprites.
	/// \param hint  If you know in advance how many sprites you'll need, set
	///              \p hint for more efficient storage.
	SpriteBatch(const size_t hint = 8);

	/// Make a deep copy of a SpriteBatch.
	SpriteBatch(const SpriteBatch &);

	~SpriteBatch();

	/// Add a textured sprite to the batch given texture coordinates.
	/// \param x,y     Position of the texture assigned to the sprite.
	/// \param width   Width of the texture and, consequently, the sprite.
	/// \param height  Height of the texture and, consequently, the sprite.
	/// \return The newly created sprite, at position (0,0).
	Sprite* add(const int x, const int y, const int width, const int height);

	/// Create a sprite.
	/// \param width   Width of the sprite.
	/// \param height  Height of the sprite.
	/// \return The newly created sprite, at position (0,0).
	Sprite* create_sprite(const unsigned int width, const unsigned int height);

	/// Load texture data.
	TextureAtlas* set_texture(const DataMap &texture);

	/// Re-use a texture atlas.
	TextureAtlas* set_texture(TextureAtlas *texture);

	/// Draw the batch of sprites.
	void draw();

	/// Update the batch of sprites.
	void update();

private:
	Renderer::VertexArray array;

	SharedPtr<TextureAtlas> texture;  ///< Texture atlas used by all sprites in the batch.
	Vector<Sprite*> sprites;          ///< Vector storing all sprites.
	Vector<SpriteVertex> vertices;    ///< Common vertex array for all sprites in the batch.

	/// One does not simply assign a SpriteBatch.
	SpriteBatch& operator=(const SpriteBatch &);
};

#endif
