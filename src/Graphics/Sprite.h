#ifndef SPRITE_H_
#define SPRITE_H_

#include "Common/Vec2.h"
#include "Graphics/Transition.h"

class  SpriteBatch;
struct SpriteVertex;

/// Custom sprite object, created by TextureAtlas.
///
/// Implemented using interleaved vertex data buffer objects.
///
/// FIXME: Transitions (move, rotate, scale) does a lot of memory juggling.
///        Perhaps implement some kind of memory pooling?
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
/// \see http://mathforum.org/mathimages/index.php/Transformation_Matrix
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Sprite
{
	friend class SpriteBatch;

public:
	const unsigned int width;   ///< Width of sprite (not scaled)
	const unsigned int height;  ///< Height of sprite (not scaled)

	Sprite(const unsigned int width, const unsigned int height, const SpriteBatch *parent);
	Sprite(const Sprite &);

	/// Return the current angle of the sprite.
	const float& get_angle() const;

	/// Return the current position of the sprite.
	const Vec2f& get_position() const;

	/// Move sprite by (x,y).
	void move(const float x, const float y);

	/// Rotate sprite by r.
	void rotate(const float r);

	void move(const float x, const float y, const unsigned int duration, const int trns_x = 0, const int trns_y = 0);
	void rotate(const float r, const unsigned int duration, const int trns = 0);

	/// Set sprite colour.
	/// \param v0  Colour for vertex 0
	/// \param v1  Colour for vertex 1
	/// \param v2  Colour for vertex 2
	/// \param v3  Colour for vertex 3
	void set_color(const unsigned int v0, const unsigned int v1, const unsigned int v2, const unsigned int v3);

	/// Set the pivot point for rotation and translation.
	/// \param x  Normalised x-component of pivot point
	/// \param y  Normalised y-component of pivot point
	void set_pivot(const float x, const float y);

	/// Set sprite position (absolute).
	void set_position(const float x, const float y);

	/// Set sprite position (absolute).
	void set_position(const Vec2f &);

	/// Set angle of rotation (in radian).
	void set_rotation(const float);

	/// Uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const float);

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	void set_scale(const Vec2f &);

	/// Set the texture.
	/// \param id  Id of texture to use
	void set_texture(const unsigned int id);

	/// Update the vertices of this sprite.
	void update();

private:
	bool buffered;               ///< Whether or not this sprite is buffered
	unsigned char stale;         ///< Sprite is stale if its properties has changed

	float angle;                 ///< Sprite rotation angle
	float cos_r;                 ///< Cosine of angle
	float sin_r;                 ///< Sine of angle

	Transition *transitions[4];  ///< Container for the three possible transitions: movement, rotation and scaling

	SpriteVertex *vertex_array;  ///< Vertex array or, if buffered, the sprite batch's buffer
	const SpriteBatch *parent;   ///< Pointer to sprite batch

	Vec2f pivot;                 ///< Pivot point (normalised)
	Vec2f position;              ///< Current position
	Vec2f position_d;            ///< Difference between current and next position
	Vec2f scale_f;               ///< Scaling factor
	Vec2f origin[4];             ///< Original rendering at origo

	void delete_transitions();
	void do_transition(const unsigned int i, const unsigned char mask);
	Sprite& operator=(const Sprite &);
};

inline const float& Sprite::get_angle() const
{
	return this->angle;
}

inline const Vec2f& Sprite::get_position() const
{
	return this->position;
}

inline void Sprite::do_transition(const unsigned int i, const unsigned char mask)
{
	if (this->transitions[i])
	{
		if (this->transitions[i]->update())
		{
			delete this->transitions[i];
			this->transitions[i] = 0;
		}
		this->stale |= mask;
	}
}

#endif
