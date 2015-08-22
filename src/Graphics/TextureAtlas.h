// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREATLAS_H_
#define GRAPHICS_TEXTUREATLAS_H_

#include <vector>

#include "Common/DataMap.h"
#include "Graphics/Texture.h"
#include "Memory/SharedPtr.h"

/// <summary>Texture atlas loaded from an image file.</summary>
/// <remarks>
///   <list type="bullet">
///     <item>
///       Textures are loaded "upside-down", so the coordinates must be flipped.
///     </item>
///     <item>
///       iOS: Textures' dimension must be 2<sup>n</sup> by 2<sup>m</sup> for
///       some arbitrary n and m, where n, m > 6.
///     </item>
///   </list>
///   References
///   <list type="bullet">
///     <item>http://iphonedevelopment.blogspot.com/2009/05/opengl-es-from-ground-up-part-6_25.html</item>
///     <item>http://developer.android.com/guide/topics/resources/providing-resources.html</item>
///     <item>http://en.wikibooks.org/wiki/OpenGL_Programming/Intermediate/Textures</item>
///   </list>
/// </remarks>
class TextureAtlas : public RefCounted
{
public:
	explicit TextureAtlas(const DataMap &img);
	~TextureAtlas();

	int width() const { return width_; }
	int height() const { return height_; }
	bool is_valid() const { return name_; }
	size_t size() const { return regions_.size(); }

	/// <summary>Binds this texture.</summary>
	void bind() const;
	void bind(const unsigned int unit) const;

	/// <summary>Defines a texture region.</summary>
	/// <param name="origin">Starting point of the region.</param>
	/// <param name="width">Width of the region.</param>
	/// <param name="height">Height of the region.</param>
	/// <returns>The id of the region.</returns>
	unsigned int define(const Vec2i &origin, const int width, const int height);

	/// <summary>Trims the internal texture region storage.</summary>
	void trim() { regions_.shrink_to_fit(); }

	const Texture& operator[](const unsigned int i) const
	{
		return regions_[i];
	}

private:
	unsigned int name_;             ///< Texture atlas' id.
	int width_;                     ///< Width of texture atlas.
	int height_;                    ///< Height of texture atlas.
	std::vector<Texture> regions_;  ///< Defined texture regions.
};

#endif
