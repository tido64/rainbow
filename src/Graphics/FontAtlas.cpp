// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "Common/Data.h"
#include "Graphics/FontAtlas.h"
#include "Graphics/OpenGL.h"

FontAtlas::FontAtlas(const float pt) : height(0), pt(pt), texture(0)
{
	for (size_t i = 0; i < chars; ++i)
		this->charset[i].code = i + ascii_offset;

#if FONTATLAS_EXTENDED > 0

	const unsigned long characters[] = {
		0x00c5,  // LATIN CAPITAL LETTER A WITH RING ABOVE
		0x00c6,  // LATIN CAPITAL LETTER AE
		0x00d8,  // LATIN CAPITAL LETTER O WITH STROKE
		0x00e5,  // LATIN SMALL LETTER A WITH RING ABOVE
		0x00e6,  // LATIN SMALL LETTER AE
		0x00f8,  // LATIN SMALL LETTER O WITH STROKE
		0
	};

	for (size_t i = 0; characters[i]; ++i)
		this->charset[chars + i].code = characters[i];

#endif
}

bool FontAtlas::load(const Data &font)
{
	R_ASSERT(font, "No data provided");

	// Instantiate FreeType
	FT_Library lib;
	if (FT_Init_FreeType(&lib))
	{
		R_ASSERT(false, "Failed to initialise FreeType");
		return false;
	}

	// Load font face
	FT_Face face;
	if (FT_New_Memory_Face(lib, static_cast<const FT_Byte*>(font.bytes()), font.size(), 0, &face))
	{
		FT_Done_FreeType(lib);
		R_ASSERT(false, "Failed to load font face");
		return false;
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
	{
		FT_Done_Face(face);
		FT_Done_FreeType(lib);
		R_ASSERT(false, "Failed to select character map");
		return false;
	}

	FT_Set_Char_Size(face, 0, static_cast<int>(this->pt * 64), 96, 96);

	// Naive algorithm for calculating texture size
	unsigned int max_width = 0;
	unsigned int max_height = 0;
	for (unsigned int i = 0; i < chars; ++i)
	{
		if (FT_Load_Char(face, i + ascii_offset, FT_LOAD_RENDER))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(lib);
			R_ASSERT(false, "Failed to load characters");
			return false;
		}

		const FT_Bitmap &bitmap = face->glyph->bitmap;

		const unsigned int width = bitmap.width + (padding << 1);
		if (width > max_width)
			max_width = width;

		const unsigned int height = bitmap.rows + (padding << 1);
		if (height > max_height)
			max_height = height;
	}
	const unsigned int tex_width = Rainbow::next_pow2(max_width * 12);
	const unsigned int tex_height = Rainbow::next_pow2(max_height * 12);

	// GL_LUMINANCE8_ALPHA8 buffer
	unsigned int w_offset = (tex_width * tex_height) << 1;
	GLubyte *tex_buf = new GLubyte[w_offset];
	memset(tex_buf, 0, w_offset);

	// Read all glyph bitmaps and copy them to our texture
	w_offset = 0;
	unsigned int h_offset = 0;
	const float tex_w_fraction = 1.0f / tex_width;
	const float tex_h_fraction = 1.0f / tex_height;
	for (unsigned int i = 0; i < chars + FONTATLAS_EXTENDED; ++i)
	{
		const unsigned int idx = FT_Get_Char_Index(face, this->charset[i].code);
		FT_Load_Glyph(face, idx, FT_LOAD_RENDER);

		const FT_GlyphSlot &slot = face->glyph;
		const FT_Bitmap &bitmap = slot->bitmap;

		// Make sure bitmap data has enough space
		const int width = bitmap.width + (padding << 1);
		if (w_offset + width > tex_width)
		{
			w_offset = 0;
			h_offset += max_height;
		}

		// Copy bitmap data to texture
		if (bitmap.buffer)
		{
			const unsigned char *buf = bitmap.buffer;
			unsigned char *d_ptr = tex_buf + ((h_offset * tex_width + w_offset) << 1) + ((padding << 1) * (tex_width + 1));
			unsigned int tmp = (tex_width - bitmap.width) * 2;
			for (int y = 0; y < bitmap.rows; ++y)
			{
				for (int x = 0; x < bitmap.width; ++x)
				{
				#ifdef RAINBOW_IOS

					*d_ptr = *(d_ptr + 1) = *buf;

				#else

					if (*buf)
					{
						*d_ptr = 255;
						*(d_ptr + 1) = *buf;
					}

				#endif

					d_ptr += 2;
					++buf;
				}
				d_ptr += tmp;
			}
		}

		// Save font glyph
		FontGlyph &fg = this->charset[i];
		fg.advance = static_cast<short>(slot->advance.x >> 6);
		fg.left = slot->bitmap_left;

	#ifdef FONTATLAS_KERNING

		if (FT_HAS_KERNING(face))
		{
			unsigned int glyph = i + ascii_offset;
			FT_Vector kerning;
			for (unsigned int j = 0; j < chars; ++j)
			{
				FT_Get_Kerning(face, j + ascii_offset, glyph, FT_KERNING_DEFAULT, &kerning);
				fg.kern[j] = static_cast<short>(kerning.x >> 6);
			}
		}

	#endif

		fg.quad[0].position.x = -margin;
		fg.quad[0].position.y = static_cast<float>(slot->bitmap_top - bitmap.rows - margin);
		fg.quad[1].position.x = static_cast<float>(bitmap.width + margin);
		fg.quad[1].position.y = fg.quad[0].position.y;
		fg.quad[2].position.x = fg.quad[1].position.x;
		fg.quad[2].position.y = static_cast<float>(slot->bitmap_top + margin);
		fg.quad[3].position.x = fg.quad[0].position.x;
		fg.quad[3].position.y = fg.quad[2].position.y;

		fg.quad[0].texcoord.x = (padding - margin + w_offset) * tex_w_fraction;
		fg.quad[0].texcoord.y = (padding + bitmap.rows + margin + h_offset) * tex_h_fraction;
		fg.quad[1].texcoord.x = (padding + bitmap.width + margin + w_offset) * tex_w_fraction;
		fg.quad[1].texcoord.y = fg.quad[0].texcoord.y;
		fg.quad[2].texcoord.x = fg.quad[1].texcoord.x;
		fg.quad[2].texcoord.y = (padding - margin + h_offset) * tex_h_fraction;
		fg.quad[3].texcoord.x = fg.quad[0].texcoord.x;
		fg.quad[3].texcoord.y = fg.quad[2].texcoord.y;

		// Advance to next "slot" in texture
		w_offset += width;
	}
	this->height = face->size->metrics.height / 64;
	FT_Done_Face(face);
	FT_Done_FreeType(lib);

	/**
	 * For printing out texture buffer
	 *
	const GLubyte *i = tex_buf;
	for (unsigned int y = 0; y < tex_height; ++y)
	{
		for (unsigned int x = 0; x < tex_width; ++x)
		{
			printf("%c", (*i > 0) ? 'X' : ' ');
			i += 2;
		}
		puts("");
	}
	 */

	this->texture = TextureManager::Instance().create(GL_LUMINANCE_ALPHA, tex_width, tex_height, GL_LUMINANCE_ALPHA, tex_buf);
	delete[] tex_buf;
	return true;
}
