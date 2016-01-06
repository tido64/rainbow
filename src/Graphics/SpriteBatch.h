// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITEBATCH_H_
#define GRAPHICS_SPRITEBATCH_H_

#include "Graphics/Buffer.h"
#include "Graphics/Sprite.h"
#include "Graphics/TextureAtlas.h"
#include "Graphics/VertexArray.h"
#include "Memory/Arena.h"

namespace rainbow { struct ISolemnlySwearThatIAmOnlyTesting; }

/// <summary>A drawable batch of sprites.</summary>
/// <remarks>
///   All sprites share a common vertex buffer object (at different offsets) and
///   are drawn with a single glDraw call. The sprites must use the same texture
///   atlas.
/// </remarks>
class SpriteBatch : private NonCopyable<SpriteBatch>
{
public:
	/// <summary>Creates a batch of sprites.</summary>
	/// <param name="hint">
	///   If you know in advance how many sprites you'll need, set
	///   <paramref name="hint"/> for more efficient storage.
	/// </param>
	SpriteBatch(unsigned int hint = 4);
	explicit SpriteBatch(const rainbow::ISolemnlySwearThatIAmOnlyTesting&);
	SpriteBatch(SpriteBatch&&);
	~SpriteBatch();

	/// <summary>Returns a pointer to the beginning.</summary>
	auto begin() const -> Sprite* { return sprites(); }

	/// <summary>Returns a pointer to the end.</summary>
	auto end() const -> Sprite* { return sprites() + count_; }

	/// <summary>Returns whether the batch is visible.</summary>
	auto is_visible() const { return visible_; }

	/// <summary>Returns current normal map.</summary>
	auto normal() const -> const TextureAtlas&
	{
		R_ASSERT(normal_.get(), "Normal texture is not set");
		return *normal_.get();
	}

	/// <summary>Returns sprite count.</summary>
	auto size() const { return count_; }

	/// <summary>Returns the sprites array.</summary>
	auto sprites() const -> Sprite* { return sprites_.get(); }

	/// <summary>Returns current texture.</summary>
	auto texture() const -> const TextureAtlas&
	{
		R_ASSERT(texture_.get(), "Texture is not set");
		return *texture_.get();
	}

	/// <summary>Returns the vertex array object.</summary>
	auto vertex_array() const -> const VertexArray& { return array_; }

	/// <summary>Returns the vertex count.</summary>
	auto vertex_count() const { return !visible_ ? 0 : count_ * 6; }

	/// <summary>Assigns a normal map.</summary>
	void set_normal(SharedPtr<TextureAtlas> texture);

	/// <summary>Assigns a texture atlas.</summary>
	void set_texture(SharedPtr<TextureAtlas> texture);

	/// <summary>Sets batch visibility.</summary>
	void set_visible(bool visible) { visible_ = visible; }

	/// <summary>
	///   Adds a textured sprite to the batch given texture coordinates.
	/// </summary>
	/// <param name="x">
	///   Origin (x-component) of the texture assigned to the sprite.
	/// </param>
	/// <param name="y">
	///   Origin (y-component) of the texture assigned to the sprite.
	/// </param>
	/// <param name="width">Width of the texture and thus the sprite.</param>
	/// <param name="height">Height of the texture and thus the sprite.</param>
	/// <returns>
	///   Reference to the newly created sprite, positioned at (x,y).
	/// </returns>
	Sprite::Ref add(int x, int y, int width, int height);

	/// <summary>Binds all used textures.</summary>
	void bind_textures() const;

	/// <summary>Brings sprite to front.</summary>
	/// <remarks>Invalidates all references.</remarks>
	void bring_to_front(const Sprite::Ref&);

	/// <summary>Clears all sprites.</summary>
	void clear() { count_ = 0; }

	/// <summary>Creates a sprite.</summary>
	/// <param name="width">Width of the sprite.</param>
	/// <param name="height">Height of the sprite.</param>
	/// <returns>
	///   Reference to the newly created sprite, positioned at (0,0).
	/// </returns>
	Sprite::Ref create_sprite(unsigned int width, unsigned int height);

	/// <summary>Erases a sprite from the batch.</summary>
	/// <remarks>Invalidates all references.</remarks>
	void erase(const Sprite::Ref&);

	/// <summary>Returns the first sprite with the given id.</summary>
	Sprite::Ref find_sprite_by_id(int id) const;

	/// <summary>Moves all sprites by (x,y).</summary>
	void move(const Vec2f&);

	/// <summary>Swaps two sprites' positions in the batch.</summary>
	void swap(const Sprite::Ref&, const Sprite::Ref&);

	/// <summary>Updates the batch of sprites.</summary>
	void update();

#ifdef RAINBOW_TEST
	auto capacity() const { return reserved_; }
	auto vertices() const { return vertices_.get(); }
#endif

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
	bool visible_;                     ///< Whether the batch is visible.

	/// <summary>Sets the array state for this batch.</summary>
	void bind_arrays() const;

	/// <summary>
	///   Resizes all client buffers to <paramref name="size"/>.
	/// </summary>
	void resize(unsigned int size);

	/// <summary>Performs a left rotation on a range of sprites.</summary>
	void rotate(size_t first, size_t n_first, size_t last);

	/// <summary>Sets all sprites to use <paramref name="buffer"/>.</summary>
	template <typename T>
	void set_buffer(T* buffer);
};

#endif
