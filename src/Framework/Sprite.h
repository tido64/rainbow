/// Custom sprite object, created by TextureAtlas.

/// Implemented using interleaved vertex data buffer objects.
///
/// Affine matrix combines scaling+translation and rotation:
///
/// | sx  0   Tx |   | cos(θ) -sin(θ)  0 |   | sx*cos(θ)  -sx*sin(θ)  Tx |
/// | 0   sy  Ty | x | sin(θ)  cos(θ)  0 | = | sy*sin(θ)   sx*cos(θ)  Ty |
/// | 0   0   1  |   |   0       0     1 |   |     0           0       1 |
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
/// \see http://mathforum.org/mathimages/index.php/Transformation_Matrix
///
/// Copyright 2010 Ninja Unicorn. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITE_H_
#define SPRITE_H_

#include <cassert>
#include <cmath>
#include <cfloat>
#include <OpenGLES/ES1/gl.h>

static const unsigned int sprite_vertex_sz = 4;
static const unsigned int sprite_vertices = 4;
static const unsigned int sprite_vertex_array = sprite_vertices * sprite_vertex_sz;
static const unsigned int sprite_buffer_sz = sprite_vertex_array * sizeof(float);
static const unsigned int sprite_buffer_stride = sprite_vertices * sizeof(float);

class TextureAtlas;

class Sprite
{
public:
	const unsigned int width;   ///< Width of sprite (not scaled)
	const unsigned int height;  ///< Height of sprite (not scaled)

	Sprite(TextureAtlas *, const unsigned int w, const unsigned int h);
	~Sprite();

	/// Draw the sprite.
	void draw();

	/// Rotate the sprite by an angle (in radian).
	void rotate(const float r);

	/// Uniform scaling of sprite (does not affect width and height properties).
	void scale(const float f);

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	void scale(const float fx, const float fy);

	/// Sets the pivot point for rotation and translation.
	/// \param x  Normalised x-component of pivot point
	/// \param y  Normalised y-component of pivot point
	void set_pivot(const float x, const float y);

	/// Sets sprite position (absolute).
	void set_position(const float x, const float y);

	/// Sets the texture.
	/// \param id  Id of texture to use
	void set_texture(const unsigned int id);

	/// Updates the vertices of this sprite.
	void update();

	/// Compares to floating point numbers. Returns true on approximately equal.
	static inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

private:
	bool batched;         ///< Whether or not this sprite is batched
	unsigned char stale;  ///< Sprite is stale if its properties has changed
	GLuint texture;       ///< Texture buffer name

	float angle;          ///< Sprite rotation angle
	float cos_r;          ///< Cosine of angle
	float sin_r;          ///< Sine of angle

	float scale_x;        ///< Scale in x-direction
	float scale_y;        ///< Scale in y-direction

	float origin[8];      ///< The original rendering at origo
	float pivot_x;        ///< x-component of the sprite's pivot point (normalised)
	float pivot_y;        ///< y-component of the sprite's pivot point (normalised)
	float position_dx;    ///< Difference between current and next position
	float position_dy;    ///< Difference between current and next position
	float position_x;     ///< x-component of the sprite's position
	float position_y;     ///< y-component of the sprite's position

	float *vertex_array;  ///< Vertex array
	TextureAtlas *atlas;  ///< Texture atlas pointer

	template<int N> friend class SpriteBatch;
	friend class TextureAtlas;
};

#endif
