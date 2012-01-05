#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Common/Vec2.h"
#include "Common/Vector.h"
#include "Graphics/OpenGL.h"

class Data;

/// A texture loaded from an image file.
///
/// iOS Note: Textures' dimension must be (2^n) by (2^m) for some arbitrary n
/// and m, where n, m > 6.
///
/// Note: Textures are loaded upside-down, so the coordinates must be flipped.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
/// \see http://developer.android.com/guide/topics/resources/providing-resources.html
/// \see http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Texture
{
	template<class T> friend class SmartPtr;

public:
	GLuint name;

	explicit Texture(const Data &img);

	/// Create a texture from file.
	/// \param x       Starting point of the texture (x-coordinate)
	/// \param y       Starting point of the texture (y-coordinate)
	/// \param width   Width of the texture
	/// \param height  Height of the texture
	/// \return The name of the texture
	unsigned int create(const int x, const int y, const int width, const int height);

	/// Trim the internal texture storage.
	inline void trim();

	inline const Vec2f* operator[](const unsigned int i) const;

private:
	unsigned int refs;
	GLsizei width, height;
	Vector<Vec2f> textures;  ///< Texture coordinates

	/// Return true if the integer provided is a power of 2.
	bool is_pow2(const unsigned int);
};

void Texture::trim()
{
	this->textures.reserve(0);
}

const Vec2f* Texture::operator[](const unsigned int i) const
{
	return &this->textures[i];
}

#endif
