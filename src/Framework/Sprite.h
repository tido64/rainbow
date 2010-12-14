/// Custom sprite object, created by TextureAtlas.

/// Implemented using interleaved vertex data buffer objects.
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/TechniquesforWorkingwithVertexData/TechniquesforWorkingwithVertexData.html
/// \see http://iphonedevelopment.blogspot.com/2009/06/opengl-es-from-ground-up-part-8.html
///
/// Copyright 2010 Ninja Unicorn. All rights reserved.
/// \author Tommy Nguyen

#ifndef SPRITE_H_
#define SPRITE_H_

#include <cassert>
#include <OpenGLES/ES1/gl.h>

class TextureAtlas;

class Sprite
{
public:
	bool visible;               ///< Visibility of sprite
	const unsigned int width;   ///< Width of sprite (not scaled)
	const unsigned int height;  ///< Height of sprite (not scaled)
	GLuint texture;             ///< Texture buffer name
	float rotation;             ///< Sprite rotation
	float scale_x;              ///< Scale in x-direction
	float scale_y;              ///< Scale in y-direction

	Sprite(TextureAtlas *, const unsigned int w, const unsigned int h);

	void draw();

	/// Uniform scaling of sprite (does not affect width and height properties).
	inline void scale(const float f)
	{
		if (f == this->scale_x && f == this->scale_y) return;

		this->scale_x = f;
		this->scale_y = f;
		this->update();
	}

	/// Non-uniform scaling of sprite (does not affect width and height properties).
	inline void scale(const float fx, const float fy)
	{
		if (fx == this->scale_x && fy == this->scale_y) return;

		this->scale_x = fx;
		this->scale_y = fy;
		this->update();
	}

	/// Sets the pivot point for rotation and translation.
	/// \param x  Normalised x-component of pivot point
	/// \param y  Normalised y-component of pivot point
	inline void set_pivot(const float x, const float y)
	{
		assert(x >= 0.0f && x <= 1.0f);
		assert(y >= 0.0f && y <= 1.0f);

		const float x0 = this->position_x - this->pivot_delta[0];
		const float y0 = this->position_y - this->pivot_delta[1];

		this->pivot_delta[0] = this->width * x;
		this->pivot_delta[1] = this->height * (1 - y);

		this->pivot_abs[0] = x0 + this->pivot_delta[0];
		this->pivot_abs[1] = y0 + this->pivot_delta[1];

		this->pivot_x = x;
		this->pivot_y = y;
	}

	/// Sets sprite position (absolute).
	void set_position(const float x, const float y);

	/// Sets the texture.
	/// \param id  Texture name to use
	void set_texture(const unsigned int id, GLenum usage = GL_DYNAMIC_DRAW);

private:
	static const unsigned int vertex_sz = 4;
	static const unsigned int vertices = 4;
	static const unsigned int buffer_sz = vertices * vertex_sz * sizeof(float);

	//GLuint buffer;                             ///< Vertex buffer object name
	float pivot_abs[2];                        ///< Absolute pivot point coordinates
	float pivot_delta[2];                      ///< For storing axis components
	float pivot_x;                             ///< x-component of the sprite's pivot point (normalised)
	float pivot_y;                             ///< y-component of the sprite's pivot point (normalised)
	float position_x;                          ///< x-component of the sprite's position
	float position_y;                          ///< y-component of the sprite's position
	float vertex_array[vertices * vertex_sz];  ///< Vertex array
	TextureAtlas *atlas;                       ///< Texture atlas pointer

	/// Updates vertex buffer object.
	void update();

	template<int N> friend class SpriteBatch;
	friend class TextureAtlas;
};

#endif
