// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include "Common/DataMap.h"
#include "Common/SharedPtr.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"

/// Texture atlas loaded from an image file.
///
/// \note Textures are loaded "upside-down", so the coordinates must be flipped.
/// \note iOS: Textures' dimension must be \c 2<sup>n</sup> by \c 2<sup>m</sup>
///       for some arbitrary \c n and \c m, where <tt>n, m > 6</tt>.
///
/// \see http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html
///      http://developer.android.com/guide/topics/resources/providing-resources.html
///      http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures
class TextureAtlas : public RefCounted
{
public:
	explicit TextureAtlas(const DataMap &img);
	inline ~TextureAtlas();

	/// Binds this texture.
	inline void bind() const;
	inline void bind(const unsigned int unit) const;

	/// Defines a texture within the atlas.
	/// \param x,y     Starting point of the texture.
	/// \param width   Width of the texture.
	/// \param height  Height of the texture.
	/// \return The id of the texture.
	unsigned int define(const Vec2i &origin, const int width, const int height);

	/// Trims the internal texture storage.
	inline void trim();

	inline const Texture& operator[](const unsigned int i) const;
	inline explicit operator bool() const;

private:
	unsigned int name;         ///< Texture atlas' id.
	int width;                 ///< Width of texture atlas.
	int height;                ///< Height of texture atlas.
	Vector<Texture> textures;  ///< Defined textures.
};

TextureAtlas::~TextureAtlas()
{
	TextureManager::Instance->remove(this->name);
}

void TextureAtlas::bind() const
{
	TextureManager::Instance->bind(this->name);
}

void TextureAtlas::bind(const unsigned int unit) const
{
	TextureManager::Instance->bind(this->name, unit);
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
