#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include "Common/SmartPtr.h"
#include "Common/Vec2.h"
#include "Common/Vector.h"
#include "Graphics/OpenGL.h"

class Data;

/// Texture atlas loaded from an image file.
///
/// \note Textures are loaded "upside-down", so the coordinates must be flipped.
/// \note iOS: Textures' dimension must be \c 2<sup>n</sup> by \c 2<sup>m</sup>
///       for some arbitrary \c n and \c m, where <tt>n, m > 6</tt>.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
///      http://developer.android.com/guide/topics/resources/providing-resources.html
///      http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class TextureAtlas : public SmartPtrFriendly
{
public:
	explicit TextureAtlas(const Data &img);
	inline ~TextureAtlas();

	/// Bind this texture.
	inline void bind() const;

	/// Define a texture within the atlas.
	/// \param x,y     Starting point of the texture.
	/// \param width   Width of the texture.
	/// \param height  Height of the texture.
	/// \return The id of the texture.
	unsigned int define(const int x, const int y, const int width, const int height);

	/// Trim the internal texture storage.
	inline void trim();

	inline Vec2f* operator[](const unsigned int i) const;
	inline operator bool() const;

private:
	GLuint name;             ///< Texture atlas' GL id.
	GLsizei width;           ///< Width of texture atlas.
	GLsizei height;          ///< Height of texture atlas.
	Vector<Vec2f> textures;  ///< Texture coordinates.

	/// Return \c true if the integer is a power of 2 and not less than 64.
	bool is_valid(const unsigned int);
};

TextureAtlas::~TextureAtlas()
{
	glDeleteTextures(1, &this->name);
}

void TextureAtlas::bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->name);
}

void TextureAtlas::trim()
{
	this->textures.reserve(0);
}

Vec2f* TextureAtlas::operator[](const unsigned int i) const
{
	return &this->textures[i];
}

TextureAtlas::operator bool() const
{
	return this->name;
}

#endif
