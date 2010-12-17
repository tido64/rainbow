/// Use FreeType to load OpenType and TrueType fonts.

/// \see http://www.alfredrossi.com/?p=73
/// \see http://musingsofninjarat.wordpress.com/opengl-freetype-texture-fonts/
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef FONT_H_
#define FONT_H_

//#include <ft2build.h>
//#include FT_FREETYPE_H
//#include FT_GLYPH_H
#include <OpenGLES/ES1/gl.h>

struct char_data;

class Font
{
public:
	const unsigned int pt;

	Font(const char *font_family, unsigned int pt);
	~Font();

	void test();

private:
	static const unsigned char chars = 128;
	static const unsigned int char_sz = 16;

	GLuint textures[chars];
	GLfloat charset[chars * char_sz];

	inline int next_pow2(const int a)
	{
		int p = 2;
		for (; p < a; p <<= 1);
		return p;
	}
};

#endif
