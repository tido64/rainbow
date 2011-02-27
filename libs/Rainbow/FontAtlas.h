/// Uses FreeType to load OpenType and TrueType fonts.

/// Creates textures from font glyphs for symbols 1 through 128. Later
/// implementations should put all symbols in a single texture and optimise
/// printing on to the screen.
///
/// \see http://www.alfredrossi.com/?p=73
/// \see http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/
/// \see http://ftgl.sourceforge.net/docs/html/ftgl-tutorial.html
/// \see Beginning OpenGL Game Programming 2nd Edition (2009)
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONTATLAS_H_
#define FONTATLAS_H_

#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "AssetManager.h"
#include "FontGlyph.h"
#include "OpenGL.h"

class FontAtlas
{
public:
	const float pt;

	FontAtlas(const char *font_family, const float pt);
	~FontAtlas() { }

	/// Prints text at (x,y).
	void print(const char *text, const float x = 0.0f, const float y = 0.0f) const;

protected:
	static const unsigned char ascii_offset = 32;  ///< Start loading from character 32
	static const unsigned char chars = 95;         ///< Load characters through 126 from the ASCII table
	static const unsigned short int margin = 2;    ///< Drawn margin around font glyph
	static const unsigned short int padding = 3;   ///< Padding around font glyph texture

private:
	GLuint textures[chars];
	FontGlyph charset[chars];

	inline int next_pow2(const int a)
	{
		int p = 16;
		for (; p < a; p <<= 1);
		return p;
	}
};

#endif
