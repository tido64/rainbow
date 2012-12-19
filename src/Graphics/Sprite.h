#ifndef SPRITE_H_
#define SPRITE_H_

#include "Common/NonCopyable.h"
#include "Graphics/SpriteVertex.h"

class SpriteBatch;

/// A sprite is a textured quad.
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
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Sprite : public NonCopyable<Sprite>
{
	friend class SpriteBatch;

public:
	const unsigned int width;   ///< Width of sprite (not scaled).
	const unsigned int height;  ///< Height of sprite (not scaled).

	Sprite(const unsigned int width, const unsigned int height, const SpriteBatch *parent);
	Sprite(const Sprite &, const SpriteBatch *parent);

	/// Return the current angle of the sprite.
	inline float get_angle() const;

	/// Return vertex 0's colour.
	inline const Colorb& get_color() const;

	/// Return the current position of the sprite.
	inline const Vec2f& get_position() const;

	/// Mirror sprite.
	void mirror();

	/// Move sprite by (x,y).
	void move(const float x, const float y);

	/// Rotate sprite by r.
	void rotate(const float r);

	/// Set sprite colour.
	void set_color(const unsigned int c);

	/// Set colour of vertex \p n.
	template<size_t n>
	inline void set_color(const unsigned int c);

	/// Set the pivot point for rotation and translation.
	/// \param x,y  Normalised pivot point.
	void set_pivot(const float x, const float y);

	/// Set sprite position (absolute).
	void set_position(const float x, const float y);

	/// Set angle of rotation (in radian).
	void set_rotation(const float r);

	/// Uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const float f);

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const Vec2f &);

	/// Set the texture.
	/// \param id  Id of texture to use.
	void set_texture(const unsigned int id);

	/// Update the vertices of this sprite.
	void update();

private:
	float angle;                 ///< Angle of rotation.
	unsigned int stale;          ///< Sprite is stale if its properties have changed.

	SpriteVertex *vertex_array;  ///< Vertex array or, if buffered, the sprite batch's buffer.
	const SpriteBatch *parent;   ///< Pointer to sprite batch.

	Vec2f center;                ///< Committed position.
	Vec2f pivot;                 ///< Pivot point (normalised).
	Vec2f position;              ///< Uncommitted position.
	Vec2f scale_f;               ///< Scaling factor.
	Vec2f origin[4];             ///< Reference sprite at origo.
};

float Sprite::get_angle() const
{
	return this->angle;
}

const Colorb& Sprite::get_color() const
{
	return this->vertex_array[0].color;
}

const Vec2f& Sprite::get_position() const
{
	return this->position;
}

template<size_t n>
void Sprite::set_color(const unsigned int c)
{
	static_assert(n < 4, "Invalid vertex");
	this->vertex_array[n].color = c;
}

#endif
