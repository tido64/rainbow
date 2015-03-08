// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include <vector>

#include "Common/DataMap.h"
#include "Graphics/Texture.h"
#include "Memory/SharedPtr.h"

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
	~TextureAtlas();

	inline int width() const;
	inline int height() const;
	inline bool is_valid() const;
	inline size_t size() const;

	/// Binds this texture.
	void bind() const;
	void bind(const unsigned int unit) const;

	/// Defines a texture region.
	/// \param x,y     Starting point of the region.
	/// \param width   Width of the region.
	/// \param height  Height of the region.
	/// \return The id of the region.
	unsigned int define(const Vec2i &origin, const int width, const int height);

	/// Trims the internal texture region storage.
	inline void trim();

	inline const Texture& operator[](const unsigned int i) const;

private:
	unsigned int name_;             ///< Texture atlas' id.
	int width_;                     ///< Width of texture atlas.
	int height_;                    ///< Height of texture atlas.
	std::vector<Texture> regions_;  ///< Defined texture regions.
};

int TextureAtlas::width() const
{
	return width_;
}

int TextureAtlas::height() const
{
	return height_;
}

bool TextureAtlas::is_valid() const
{
	return name_;
}

size_t TextureAtlas::size() const
{
	return regions_.size();
}

void TextureAtlas::trim()
{
	regions_.shrink_to_fit();
}

const Texture& TextureAtlas::operator[](const unsigned int i) const
{
	return regions_[i];
}

#endif
