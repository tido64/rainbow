// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include "Graphics/Buffer.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"

/// A drawable batch of sprites.
///
/// All sprites share a common vertex buffer object (at different offsets) and
/// are drawn with a single glDraw call. The sprites must use the same texture
/// atlas.
class SpriteBatch : private NonCopyable<SpriteBatch>
{
public:
	/// Creates a batch of sprites.
	/// \param hint  If you know in advance how many sprites you'll need, set
	///              \p hint for more efficient storage.
	SpriteBatch(const size_t hint = 4);

	/// Returns the vertex count.
	inline size_t count() const;

	/// Returns current normal map.
	inline const TextureAtlas& normal() const;

	/// Returns a reference to the sprite at index \p i.
	inline Sprite::Ref sprite(const size_t i) const;

	/// Returns the sprites vector.
	inline const Vector<Sprite>& sprites() const;

	/// Returns current texture.
	inline const TextureAtlas& texture() const;

	/// Returns the vertex array object.
	inline const VertexArray& vertex_array() const;

	/// Assigns a normal map.
	void set_normal(TextureAtlas *texture);

	/// Assigns a texture atlas.
	void set_texture(TextureAtlas *texture);

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
	unsigned int create_sprite(const unsigned int width,
	                           const unsigned int height);

	/// Updates the batch of sprites.
	void update();

private:
	SharedPtr<TextureAtlas> normal_;   ///< Normal map used by all sprites in the batch.
	SharedPtr<TextureAtlas> texture_;  ///< Texture atlas used by all sprites in the batch.
	Vector<Sprite> sprites_;           ///< Vector storing all sprites.
	Buffer<Vec2f> normals_;            ///< Shared normal buffer.
	Buffer<SpriteVertex> vertices_;    ///< Shared, interleaved vertex buffer.
	VertexArray array_;                ///< Vertex array object.

	/// Sets the array state for this batch.
	int bind() const;
};

size_t SpriteBatch::count() const
{
	const size_t count = this->vertices_.storage().size();
	return count + (count >> 1);
}

const TextureAtlas& SpriteBatch::normal() const
{
	R_ASSERT(this->normal_.get(), "Normal texture is not set");
	return *this->normal_.get();
}

Sprite::Ref SpriteBatch::sprite(const size_t i) const
{
	return Sprite::Ref(*this, i);
}

const Vector<Sprite>& SpriteBatch::sprites() const
{
	return this->sprites_;
}

const TextureAtlas& SpriteBatch::texture() const
{
	R_ASSERT(this->texture_.get(), "Texture is not set");
	return *this->texture_.get();
}

const VertexArray& SpriteBatch::vertex_array() const
{
	return this->array_;
}

#endif
