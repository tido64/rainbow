#ifndef TEXTUREATLAS_H_
#define TEXTUREATLAS_H_

#include "Common/SharedPtr.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"

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
class TextureAtlas : public RefCounted
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
	unsigned int define(const Vec2i &origin, const int width, const int height);

	/// Trim the internal texture storage.
	inline void trim();

	inline const Texture& operator[](const unsigned int i) const;
	inline operator bool() const;

private:
	unsigned int name;         ///< Texture atlas' id.
	int width;                 ///< Width of texture atlas.
	int height;                ///< Height of texture atlas.
	Vector<Texture> textures;  ///< Defined textures.
};

TextureAtlas::~TextureAtlas()
{
	TextureManager::Instance().remove(this->name);
}

void TextureAtlas::bind() const
{
	TextureManager::Instance().bind(this->name);
}

void TextureAtlas::trim()
{
	this->textures.reserve(0);
}

const Texture& TextureAtlas::operator[](const unsigned int i) const
{
	return this->textures[i];
}

TextureAtlas::operator bool() const
{
	return this->name;
}

#endif
