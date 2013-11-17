// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_FONTATLAS_H_
#define GRAPHICS_FONTATLAS_H_
#define FONTATLAS_EXTENDED 6
//#define FONTATLAS_KERNING

#include "Common/SharedPtr.h"
#include "Graphics/FontGlyph.h"
#include "Graphics/TextureManager.h"

class Data;

/// Uses FreeType to load OpenType and TrueType fonts.
///
/// Creates textures from font glyphs for symbols 32 through 126. Copies bitmap
/// data into a larger buffer which is then used as texture.
///
/// Features:
/// - Anti-aliasing
/// - Kerning
///
/// TODO: Optimise texture size by sorting glyphs.
///
/// \see http://iphone-3d-programming.labs.oreilly.com/ch07.html
/// \see http://www.alfredrossi.com/?p=73
/// \see http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/
/// \see http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html
/// \see Beginning OpenGL Game Programming 2nd Edition (2009)
class FontAtlas : public RefCounted
{
public:
	FontAtlas(const Data &font, const float pt);

	inline ~FontAtlas();

	/// Returns the line height.
	inline int height() const;

	inline void bind() const;

	/// Returns the glyph for a character.
	const FontGlyph* get_glyph(const unsigned int c) const;

	inline operator bool() const;

protected:
	static const unsigned int kNumCharacters = 95;  ///< Load characters through 126 from the ASCII table.

private:
	const float pt_;        ///< Font point size.
	int height_;            ///< Font line height.
	unsigned int texture_;  ///< Texture name.
	Colorb color_;          ///< Font colour.
	FontGlyph charset_[kNumCharacters + FONTATLAS_EXTENDED];  ///< Character set.
};

FontAtlas::~FontAtlas()
{
	TextureManager::Instance->remove(this->texture_);
}

void FontAtlas::bind() const
{
	TextureManager::Instance->bind(this->texture_);
}

int FontAtlas::height() const
{
	return this->height_;
}

FontAtlas::operator bool() const
{
	return this->texture_ != 0;
}

#endif
