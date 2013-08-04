#ifndef FONTATLAS_H_
#define FONTATLAS_H_
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
/// FIXME: Optimise texture size by sorting glyphs.
///
/// \see http://iphone-3d-programming.labs.oreilly.com/ch07.html
/// \see http://www.alfredrossi.com/?p=73
/// \see http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/
/// \see http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html
/// \see Beginning OpenGL Game Programming 2nd Edition (2009)
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class FontAtlas : public RefCounted
{
public:
	FontAtlas(const float pt);
	inline ~FontAtlas();

	inline void bind() const;

	inline const FontGlyph* get_glyph(const unsigned long c) const;
	inline short get_height() const;

	/// Load font and create a texture atlas.
	bool load(const Data &font);

protected:
	static const unsigned int kASCIIOffset = 32;    ///< Start loading from character 32.
	static const unsigned int kNumCharacters = 95;  ///< Load characters through 126 from the ASCII table.

private:
	short height;          ///< Font line height.
	const float pt;        ///< Font point size.
	unsigned int texture;  ///< Texture name.
	Colorb color;          ///< Font colour.
	FontGlyph charset[kNumCharacters + FONTATLAS_EXTENDED];  ///< Character set.
};

FontAtlas::~FontAtlas()
{
	TextureManager::Instance->remove(this->texture);
}

void FontAtlas::bind() const
{
	TextureManager::Instance->bind(this->texture);
}

const FontGlyph* FontAtlas::get_glyph(const unsigned long c) const
{
#if FONTATLAS_EXTENDED > 0

	if (c >= 0x80u)
	{
		for (size_t i = kNumCharacters; i < kNumCharacters + FONTATLAS_EXTENDED; ++i)
			if (this->charset[i].code == c)
				return &this->charset[i];
		return nullptr;
	}

#endif

	return &this->charset[static_cast<unsigned char>(c) - kASCIIOffset];
}

short FontAtlas::get_height() const
{
	return this->height;
}

#endif
