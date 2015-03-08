// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include "Graphics/Buffer.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"
#include "Memory/Arena.h"

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
	unsigned int count() const { return count_ * 6; }

	/// Returns current normal map.
	const TextureAtlas& normal() const
	{
		R_ASSERT(normal_.get(), "Normal texture is not set");
		return *normal_.get();
	}

	/// Returns the sprites array.
	Sprite* sprites() const { return sprites_.get(); }

	/// Returns current texture.
	const TextureAtlas& texture() const
	{
		R_ASSERT(texture_.get(), "Texture is not set");
		return *texture_.get();
	}

	/// Returns the vertex array object.
	const VertexArray& vertex_array() const { return array_; }

	/// Assigns a normal map.
	void set_normal(SharedPtr<TextureAtlas> texture);

	/// Assigns a texture atlas.
	void set_texture(SharedPtr<TextureAtlas> texture);

	/// Adds a textured sprite to the batch given texture coordinates.
	/// \param x,y     Position of the texture assigned to the sprite.
	/// \param width   Width of the texture and, consequently, the sprite.
	/// \param height  Height of the texture and, consequently, the sprite.
	/// \return Reference to the newly created sprite, positioned at (x,y).
	Sprite::Ref add(const int x, const int y, const int width, const int height);

	/// Binds all used textures.
	void bind_textures() const;

	/// Creates a sprite.
	/// \param width   Width of the sprite.
	/// \param height  Height of the sprite.
	/// \return Reference to the newly created sprite, positioned at (0,0).
	Sprite::Ref create_sprite(const unsigned int width,
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
	void bind_arrays() const;

	/// Resizes all client buffers to \p size.
	void resize(const unsigned int size);

	/// Sets all sprites to use \p buffer.
	template<typename T>
	void set_buffer(T *buffer);
};

#endif
