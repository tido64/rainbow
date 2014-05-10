// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include "Common/Arena.h"
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
	SpriteBatch(const unsigned int hint = 4);
	~SpriteBatch();

	/// Returns the vertex count.
	inline unsigned int count() const;

	/// Returns current normal map.
	inline const TextureAtlas& normal() const;

	/// Returns a reference to the sprite at index \p i.
	inline Sprite::Ref sprite(const unsigned int i) const;

	/// Returns the sprites array.
	inline Sprite* sprites() const;

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

	/// Moves all sprites by (x,y).
	void move(const Vec2f &);

	/// Updates the batch of sprites.
	void update();

private:
	Arena<Sprite> sprites_;            ///< Sprite batch.
	Arena<SpriteVertex> vertices_;     ///< Client vertex buffer.
	Arena<Vec2f> normals_;             ///< Client normal buffer.
	unsigned int count_;               ///< Number of sprites.
	Buffer vertex_buffer_;             ///< Shared, interleaved vertex buffer.
	Buffer normal_buffer_;             ///< Shared normal buffer.
	VertexArray array_;                ///< Vertex array object.
	SharedPtr<TextureAtlas> normal_;   ///< Normal map used by all sprites in the batch.
	SharedPtr<TextureAtlas> texture_;  ///< Texture atlas used by all sprites in the batch.
	unsigned int reserved_;            ///< Number of sprites reserved for.

	/// Sets the array state for this batch.
	void bind() const;

	/// Resizes all client buffers to \p size.
	void resize(const unsigned int size);

	/// Sets all sprites to use \p buffer.
	template<typename T>
	void set_buffer(T *buffer);
};

unsigned int SpriteBatch::count() const
{
	return count_ * 6;
}

const TextureAtlas& SpriteBatch::normal() const
{
	R_ASSERT(normal_.get(), "Normal texture is not set");
	return *normal_.get();
}

Sprite::Ref SpriteBatch::sprite(const unsigned int i) const
{
	return Sprite::Ref(this, i);
}

Sprite* SpriteBatch::sprites() const
{
	return sprites_.get();
}

const TextureAtlas& SpriteBatch::texture() const
{
	R_ASSERT(texture_.get(), "Texture is not set");
	return *texture_.get();
}

const VertexArray& SpriteBatch::vertex_array() const
{
	return array_;
}

#endif
