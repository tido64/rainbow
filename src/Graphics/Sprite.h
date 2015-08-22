// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITE_H_
#define GRAPHICS_SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"

class SpriteBatch;

/// <summary>A textured quad.</summary>
/// <remarks>
///   <code>
///     3 ┌─────┐ 2
///       │     │
///       │     │
///     0 └─────┘ 1
///   </code>
///   <para>
///     The vertex order of the quad is 0,1,2 for the first triangle, and 2,3,0
///     for the second.
///   </para>
///   <para>
///     The sprite itself does not have a texture. It holds the texture
///     coordinates but it is the sprite batch that holds the actual texture.
///     That way, changing textures on a whole batch (i.e. skinning) can be
///     easily achieved.
///   </para>
///   <list type="bullet">
///     <item>http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html</item>
///     <item>http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html</item>
///     <item>http://mathforum.org/mathimages/index.php/Transformation_Matrix</item>
///   </list>
/// </remarks>
class Sprite : private NonCopyable<Sprite>
{
public:
	enum { kNoId };

	class Ref
	{
		friend SpriteBatch;

	public:
		Ref() : batch_(nullptr), i_(0) {}

		Sprite& operator*() const;
		Sprite* operator->() const;

		bool operator==(const Ref &other) const
		{
			return batch_ == other.batch_ && i_ == other.i_;
		}

		explicit operator bool() const { return batch_; }

	private:
		const SpriteBatch *batch_;
		size_t i_;

		Ref(const SpriteBatch *batch, const size_t i)
		    : batch_(batch), i_(i) {}
	};

	Sprite(const unsigned int width,
	       const unsigned int height,
	       const SpriteBatch *parent);
	Sprite(Sprite&&);

	float angle() const { return angle_; }
	Colorb color() const { return vertex_array_[0].color; }
	unsigned int height() const { return height_; }
	int id() const { return id_; }
	bool is_flipped() const;
	bool is_hidden() const;
	bool is_mirrored() const;
	const SpriteBatch& parent() const { return *parent_; }
	const Vec2f& pivot() const { return pivot_; }
	const Vec2f& position() const { return position_; }
	const Vec2f& scale() const { return scale_; }
	unsigned int width() const { return width_; }

#ifdef RAINBOW_TEST
	const SpriteVertex* vertex_array() const { return vertex_array_; }
#endif

	/// <summary>Sets sprite colour.</summary>
	void set_color(const Colorb c);

	/// <summary>Sets the identifier for the sprite</summary>
	void set_id(const int id) { id_ = id; }

	/// <summary>Sets normal map.</summary>
	/// <param name="id">Identifier of the normal map to set.</param>
	void set_normal(const unsigned int id);

	/// <summary>Sets buffer used to store UV coordinates.</summary>
	void set_normal_buffer(Vec2f *map) { normal_map_ = map; }

	/// <summary>Sets the pivot point for rotation and translation.</summary>
	/// <param name="p">Normalised pivot point.</param>
	void set_pivot(const Vec2f &p);

	/// <summary>Sets sprite position (absolute).</summary>
	void set_position(const Vec2f &);

	/// <summary>Sets angle of rotation (in radian).</summary>
	void set_rotation(const float r);

	/// <summary>
	///   Uniform scaling of sprite (does not affect actual width and height).
	/// </summary>
	/// <param name="f">Scaling factor for both axes.</param>
	void set_scale(const float f) { set_scale(Vec2f(f, f)); }

	/// <summary>
	///   Non-uniform scaling of sprite (does not affect actual width and
	///   height).
	/// </summary>
	/// <param name="f">Scaling factors for x- and y-axis.</param>
	void set_scale(const Vec2f &f);

	/// <summary>Sets texture.</summary>
	/// <param name="id">Identifier of the texture to set.</param>
	void set_texture(const unsigned int id);

	/// <summary>Sets vertex array buffer.</summary>
	void set_vertex_array(SpriteVertex *array);

	/// <summary>Flips sprite vertically.</summary>
	void flip();

	/// <summary>Hides sprite if it is currently shown.</summary>
	void hide();

	/// <summary>Mirrors sprite.</summary>
	void mirror();

	/// <summary>Moves sprite by (x,y).</summary>
	void move(const Vec2f &);

	/// <summary>Rotates sprite by <paramref name="r"/>.</summary>
	void rotate(const float r);

	/// <summary>Shows sprite if it is currently hidden.</summary>
	void show();

	/// <summary>Updates the vertex buffer.</summary>
	/// <returns>
	///   <c>true</c> if the buffer has changed; <c>false</c> otherwise.
	/// </returns>
	bool update();

	Sprite& operator=(Sprite&&);

private:
	unsigned int width_;          ///< Width of sprite (not scaled).
	unsigned int height_;         ///< Height of sprite (not scaled).
	unsigned int state_;          ///< State of internals, e.g. buffer.
	float angle_;                 ///< Angle of rotation.
	Vec2f pivot_;                 ///< Pivot point (normalised).
	Vec2f center_;                ///< Committed position.
	Vec2f position_;              ///< Uncommitted position.
	Vec2f scale_;                 ///< Scaling factor.
	const SpriteBatch *parent_;   ///< Pointer to sprite batch.
	SpriteVertex *vertex_array_;  ///< Interleaved vertex array.
	Vec2f *normal_map_;           ///< Normal map UV coordinates.
	int id_;                      ///< Sprite identifier.

	void flip_textures(const unsigned int axis);
	void set_normal(const unsigned int f, const Vec2f *uv);
};

#endif
