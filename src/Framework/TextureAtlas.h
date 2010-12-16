/// Custom texture atlas implemented in OpenGL ES 1.1.

/// GL drawing order: (1,1) -> (0,1) -> (1,0) -> (0,0)
///
/// Note: Textures' dimension must be (2^n) by (2^m) for some arbitrary n and m,
/// where n > 6 and m > 6.
///
/// Note: Textures are loaded upside-down, so the coordinates must be flipped.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
///
/// Copyright 2010 Ninja Unicorn. All rights reserved.
/// \author Tommy Nguyen

#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include <cstring>

#include "../Hardware/Platform.h"

#if defined(ONWIRE_ANDROID)
#
#elif defined(ONWIRE_IOS)
#	include <OpenGLES/ES1/gl.h>
#	include <UIKit/UIKit.h>
#endif

#include "Sprite.h"
#include "Vector.h"

class TextureAtlas
{
public:
	TextureAtlas(const char *filename, const int mipmap = 0);
	~TextureAtlas() { }

	/// Creates a sprite.
	/// \return A sprite object
	Sprite* create_sprite(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);

	/// Defines a texture from the map.
	/// \param x       x-component of texture location
	/// \param y       y-component of texture location
	/// \param width   Width of the texture
	/// \param height  Height of the texture
	/// \return An id unique to the texture
	unsigned int define_texture(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);

	/// Draws all sprites created from the texture atlas.
	//void draw();

	/// Returns the texture name this atlas holds.
	inline GLuint get_name() { return this->texture[0]; }

	/// Gets named texture.
	/// \param id        The id of the texture to get
	/// \param vertices  Interleaved vertex data array (vertex and texture coordinates)
	void get_texture(const unsigned int id, float *vertices);

private:
	unsigned int width;        ///< Width of the map
	unsigned int height;       ///< Height of the map
	GLuint texture[1];         ///< Stores texture name
	Vector<GLfloat> textures;  ///< Stores all textures
	Vector<Sprite *> sprites;  ///< Stores all sprites
};

#endif
