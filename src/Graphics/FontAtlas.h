// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_FONTATLAS_H_
#define GRAPHICS_FONTATLAS_H_
#define FONTATLAS_EXTENDED 6
//#define FONTATLAS_KERNING

#include "Graphics/FontGlyph.h"
#include "Graphics/Texture.h"
#include "Memory/SharedPtr.h"

class Data;
class TextureManager;

namespace rainbow { class Texture; }

/// <summary>Uses FreeType to load OpenType and TrueType fonts.</summary>
/// <remarks>
///   <para>
///     Creates textures from font glyphs for symbols 32 through 126. Copies
///     bitmap data into a larger buffer which is then used as texture.
///   </para>
///   Features:
///   <list type="bullet">
///     <item>Anti-aliasing</item>
///     <item>Kerning</item>
///   </list>
///   <para>TODO: Optimise texture size by sorting glyphs.</para>
///   References
///   <list type="bullet">
///     <item>http://iphone-3d-programming.labs.oreilly.com/ch07.html</item>
///     <item>http://www.alfredrossi.com/?p=73</item>
///     <item>http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/</item>
///     <item>http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html</item>
///     <item>Beginning OpenGL Game Programming 2nd Edition (2009)</item>
///   </list>
/// </remarks>
class FontAtlas : public RefCounted
{
public:
	FontAtlas(const char* path, float pt);
	FontAtlas(const char* name, const Data& font, float pt);

	/// <summary>Returns the line height.</summary>
	auto height() const { return height_; }

	/// <summary>Returns whether this FontAtlas is valid.</summary>
	bool is_valid() const { return texture_; }

	/// <summary>Sets this font as active texture.</summary>
	void bind() const { texture_.bind(); }

	/// <summary>Returns the glyph for character <paramref name="c"/>.</summary>
	const FontGlyph* get_glyph(unsigned int c) const;

protected:
	/// <summary>Load characters 32 through 126 from the ASCII table.</summary>
	static const unsigned int kNumCharacters = 95;

private:
	const float pt_;            ///< Font point size.
	Colorb color_;              ///< Font colour.
	int height_;                ///< Font line height.
	rainbow::Texture texture_;  ///< Texture name.
	FontGlyph charset_[kNumCharacters + FONTATLAS_EXTENDED];  ///< Character set.

	void load(TextureManager* texture_manager,
	          const rainbow::Texture& texture,
	          const Data& font);
};

#endif
