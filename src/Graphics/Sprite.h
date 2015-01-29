// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SPRITE_H_
#define GRAPHICS_SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"

class SpriteBatch;

/// A textured quad.
///
/// 3 ┌─────┐ 2
///   │     │
///   │     │
/// 0 └─────┘ 1
///
/// The vertex order of the quad is 0,1,2 for the first triangle, and 2,3,0 for
/// the second.
///
/// The sprite itself does not have a texture. It holds the texture coordinates
/// but it is the sprite batch that holds the actual texture. That way, changing
/// textures on a whole batch (i.e. skinning) can be easily achieved.
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
/// \see http://mathforum.org/mathimages/index.php/Transformation_Matrix
class Sprite : private NonCopyable<Sprite>
{
public:
	class Ref
	{
		friend SpriteBatch;

	public:
		Ref() : batch_(nullptr), i_(0) {}

		Sprite& operator*() const;
		Sprite* operator->() const;

		explicit operator bool() const { return batch_; }

	private:
		const SpriteBatch *batch_;
		size_t i_;

		Ref(const SpriteBatch *batch, const size_t i);
	};

	Sprite(const unsigned int width,
	       const unsigned int height,
	       const SpriteBatch *parent);
	Sprite(Sprite &&);

	float angle() const { return angle_; }
	Colorb color() const { return vertex_array_[0].color; }
	unsigned int height() const { return height_; }
	const SpriteBatch& parent() const { return *parent_; }
	const Vec2f& pivot() const { return pivot_; }
	const Vec2f& position() const { return position_; }
	const Vec2f& scale() const { return scale_; }
	unsigned int width() const { return width_; }

	/// Sets sprite colour.
	void set_color(const Colorb c);

	/// Sets normal map.
	/// \param id  Id of normal map to use.
	void set_normal(const unsigned int id);

	/// Sets buffer used to store UV coordinates.
	void set_normal_buffer(Vec2f *map) { normal_map_ = map; }

	/// Sets the pivot point for rotation and translation.
	/// \param x,y  Normalised pivot point.
	void set_pivot(const Vec2f &);

	/// Sets sprite position (absolute).
	void set_position(const Vec2f &);

	/// Sets angle of rotation (in radian).
	void set_rotation(const float r);

	/// Uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const float f);

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const Vec2f &);

	/// Sets the texture.
	/// \param id  Id of texture to use.
	void set_texture(const unsigned int id);

	/// Sets vertex array buffer.
	void set_vertex_array(SpriteVertex *array) { vertex_array_ = array; }

	/// Mirrors sprite.
	void mirror();

	/// Moves sprite by (x,y).
	void move(const Vec2f &);

	/// Rotates sprite by r.
	void rotate(const float r);

	/// Updates the vertices of this sprite.
	bool update();

private:
	float angle_;                 ///< Angle of rotation.
	const unsigned int width_;    ///< Width of sprite (not scaled).
	const unsigned int height_;   ///< Height of sprite (not scaled).
	unsigned int stale_;          ///< Sprite is stale if its properties have changed.

	SpriteVertex *vertex_array_;  ///< Interleaved vertex array.
	const SpriteBatch *parent_;   ///< Pointer to sprite batch.
	Vec2f *normal_map_;           ///< Normal map UV coordinates.

	Vec2f center_;                ///< Committed position.
	Vec2f pivot_;                 ///< Pivot point (normalised).
	Vec2f position_;              ///< Uncommitted position.
	Vec2f scale_;                 ///< Scaling factor.
};

#endif
