/// Uses FreeType to load OpenType and TrueType fonts.

/// \see http://www.alfredrossi.com/?p=73
/// \see http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/
/// \see Beginning OpenGL Game Programming 2nd Edition (2009)
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONTATLAS_H_
#define FONTATLAS_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <Rainbow/FontGlyph.h>
#include <Rainbow/OpenGL.h>
#include <Rainbow/Types.h>

struct char_data;

class FontAtlas
{
public:
	const unsigned int pt;

	FontAtlas(const char *font_family, const unsigned int pt);
	~FontAtlas() { }

	void print(const char *text, const float x = 0.0f, const float y = 0.0f);

protected:
	static const unsigned char chars = 128;

private:
	GLuint textures[chars];
	FontGlyph charset[chars];

	inline int next_pow2(const int a)
	{
		int p = 2;
		for (; p < a; p <<= 1);
		return p;
	}
};

#endif
