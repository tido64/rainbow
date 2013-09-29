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
/// but it is the sprite batch that holds the actual texture. That way,
/// changing textures on a whole batch (i.e. skinning) can be easily achieved.
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
/// \see http://mathforum.org/mathimages/index.php/Transformation_Matrix
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Sprite : private NonCopyable<Sprite>
{
	friend class SpriteBatch;

public:
	Sprite(const unsigned int width, const unsigned int height, const SpriteBatch *parent);
	Sprite(Sprite &&);

	/// Returns the sprite's current angle.
	inline float angle() const;

	/// Returns the sprite's colour.
	inline const Colorb& color() const;

	/// Returns the sprite's height.
	inline unsigned int height() const;

	/// Returns the sprite's current position.
	inline const Vec2f& position() const;

	/// Returns the sprite's width.
	inline unsigned int width() const;

	/// Sets sprite colour.
	void set_color(const unsigned int c);

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

	/// Mirrors sprite.
	void mirror();

	/// Moves sprite by (x,y).
	void move(const Vec2f &);

	/// Rotates sprite by r.
	void rotate(const float r);

	/// Updates the vertices of this sprite.
	void update();

private:
	float angle_;                 ///< Angle of rotation.
	const unsigned int width_;    ///< Width of sprite (not scaled).
	const unsigned int height_;   ///< Height of sprite (not scaled).
	unsigned int stale_;          ///< Sprite is stale if its properties have changed.

	SpriteVertex *vertex_array_;  ///< Vertex array or, if buffered, the sprite batch's buffer.
	const SpriteBatch *parent_;   ///< Pointer to sprite batch.

	Vec2f center_;                ///< Committed position.
	Vec2f pivot_;                 ///< Pivot point (normalised).
	Vec2f position_;              ///< Uncommitted position.
	Vec2f scale_;                 ///< Scaling factor.
};

float Sprite::angle() const
{
	return this->angle_;
}

const Colorb& Sprite::color() const
{
	return this->vertex_array_[0].color;
}

unsigned int Sprite::height() const
{
	return this->height_;
}

const Vec2f& Sprite::position() const
{
	return this->position_;
}

unsigned int Sprite::width() const
{
	return this->width_;
}

#endif
