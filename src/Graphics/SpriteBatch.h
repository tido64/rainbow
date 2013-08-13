#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include "Graphics/Drawable.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"

namespace SceneGraph { class Node; }
namespace Renderer { void draw(const SpriteBatch &); }

/// A drawable batch of sprites.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class SpriteBatch : private NonCopyable<SpriteBatch>
{
	friend class SceneGraph::Node;
	friend void Renderer::draw(const SpriteBatch &);
	friend void Sprite::set_texture(const unsigned int id);

public:
	/// Creates a batch of sprites.
	/// \param hint  If you know in advance how many sprites you'll need, set
	///              \p hint for more efficient storage.
	SpriteBatch(const size_t hint = 8);

	/// Adds a textured sprite to the batch given texture coordinates.
	/// \param x,y     Position of the texture assigned to the sprite.
	/// \param width   Width of the texture and, consequently, the sprite.
	/// \param height  Height of the texture and, consequently, the sprite.
	/// \return The index of the newly created sprite, positioned at (0,0).
	unsigned int add(const int x, const int y, const int width, const int height);

	/// Creates a sprite.
	/// \param width   Width of the sprite.
	/// \param height  Height of the sprite.
	/// \return The index of the newly created sprite, positioned at (0,0).
	unsigned int create_sprite(const unsigned int width, const unsigned int height);

	/// Loads texture data.
	TextureAtlas* set_texture(const DataMap &texture);

	/// Reuses a texture atlas.
	TextureAtlas* set_texture(TextureAtlas *texture);

	/// Returns the sprite at index \p i.
	inline Sprite& sprite(const unsigned int i) const;

	/// Updates the batch of sprites.
	void update();

private:
	Renderer::VertexArray array;

	SharedPtr<TextureAtlas> texture;  ///< Texture atlas used by all sprites in the batch.
	Vector<Sprite> sprites;           ///< Vector storing all sprites.
	Vector<SpriteVertex> vertices;    ///< Common vertex array for all sprites in the batch.
};

Sprite& SpriteBatch::sprite(const unsigned int i) const
{
	return this->sprites[i];
}

#endif
