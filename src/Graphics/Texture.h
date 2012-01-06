#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Common/SmartPtr.h"
#include "Common/Vec2.h"
#include "Common/Vector.h"
#include "Graphics/OpenGL.h"

class Data;

/// A texture atlas loaded from an image file.
///
/// \note Textures are loaded "upside-down", so the coordinates must be flipped.
///
/// \note iOS: Textures' dimension must be \c 2<sup>n</sup> by \c 2<sup>m</sup>
/// for some arbitrary \c n and \c m, where <tt>n, m > 6</tt>.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
/// \see http://developer.android.com/guide/topics/resources/providing-resources.html
/// \see http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Texture : public SmartPtrFriendly
{
public:
	GLuint name;

	explicit Texture(const Data &img);
	inline ~Texture();

	/// Define a texture within the atlas.
	/// \param x,y     Starting point of the texture.
	/// \param width   Width of the texture.
	/// \param height  Height of the texture.
	/// \return The id of the texture.
	unsigned int create(const int x, const int y, const int width, const int height);

	/// Trim the internal texture storage.
	inline void trim();

	inline const Vec2f* operator[](const unsigned int i) const;

private:
	GLsizei width, height;
	Vector<Vec2f> textures;  ///< Texture coordinates

	/// Return \c true if the integer provided is a power of 2.
	bool is_pow2(const unsigned int);
};

Texture::~Texture()
{
	glDeleteTextures(1, &this->name);
}

void Texture::trim()
{
	this->textures.reserve(0);
}

const Vec2f* Texture::operator[](const unsigned int i) const
{
	return &this->textures[i];
}

#endif
