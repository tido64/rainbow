// Copyright 2010-13 Bifrost Entertainment. All rights reserved.

#include <memory>

#include <ft2build.h>
#include FT_CFF_DRIVER_H
#include FT_GLYPH_H
#include FT_MODULE_H

#include "Common/Data.h"
#include "Graphics/FontAtlas.h"
#include "Graphics/OpenGL.h"

namespace
{
	typedef unsigned int uint_t;

	const float kGlyphMargin = 2.0f;        ///< Margin around rendered font glyph.
	const uint_t kGlyphPadding = 3;         ///< Padding around font glyph texture.
	const uint_t kNumGlyphsPerColRow = 12;  ///< Number of glyphs per column/row on texture.
}

FontAtlas::FontAtlas(const Data &font, const float pt) : height(0), pt(pt), texture(0)
{
	R_ASSERT(font, "Failed to load font");

	for (size_t i = 0; i < kNumCharacters; ++i)
		this->charset[i].code = i + kASCIIOffset;

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
		this->charset[kNumCharacters + i].code = characters[i];

#endif

	// Instantiate FreeType
	FT_Library lib;
	if (FT_Init_FreeType(&lib))
	{
		R_ASSERT(false, "Failed to initialise FreeType");
		return;
	}

	// Load font face
	FT_Face face;
	if (FT_New_Memory_Face(lib, static_cast<const FT_Byte*>(font.bytes()), font.size(), 0, &face))
	{
		FT_Done_FreeType(lib);
		R_ASSERT(false, "Failed to load font face");
		return;
	}

	if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
	{
		FT_Done_Face(face);
		FT_Done_FreeType(lib);
		R_ASSERT(false, "Failed to select character map");
		return;
	}

	FT_Set_Char_Size(face, 0, this->pt * 64, 96, 96);

	// Naive algorithm for calculating texture size
	uint_t max_width = 0;
	uint_t max_height = 0;
	for (uint_t i = 0; i < kNumCharacters; ++i)
	{
		if (FT_Load_Char(face, i + kASCIIOffset, FT_LOAD_RENDER))
		{
			FT_Done_Face(face);
			FT_Done_FreeType(lib);
			R_ASSERT(false, "Failed to load characters");
			return;
		}

		const FT_Bitmap &bitmap = face->glyph->bitmap;

		const uint_t width = bitmap.width + kGlyphPadding * 2;
		if (width > max_width)
			max_width = width;

		const uint_t height = bitmap.rows + kGlyphPadding * 2;
		if (height > max_height)
			max_height = height;
	}
	const uint_t tex_width = Rainbow::next_pow2(max_width * kNumGlyphsPerColRow);
	const uint_t tex_height = Rainbow::next_pow2(max_height * kNumGlyphsPerColRow);

	// GL_LUMINANCE8_ALPHA8 buffer
	uint_t w_offset = tex_width * tex_height * 2;
	std::unique_ptr<GLubyte[]> tex_buf(new GLubyte[w_offset]);
	memset(tex_buf.get(), 0, w_offset);

	// Read all glyph bitmaps and copy them to our texture
	w_offset = 0;
	uint_t h_offset = 0;
	const float tex_w_fraction = 1.0f / tex_width;
	const float tex_h_fraction = 1.0f / tex_height;
	for (uint_t i = 0; i < kNumCharacters + FONTATLAS_EXTENDED; ++i)
	{
		const uint_t idx = FT_Get_Char_Index(face, this->charset[i].code);
		FT_Load_Glyph(face, idx, FT_LOAD_RENDER);

		const FT_GlyphSlot &slot = face->glyph;
		const FT_Bitmap &bitmap = slot->bitmap;

		// Make sure bitmap data has enough space
		const int width = bitmap.width + kGlyphPadding * 2;
		if (w_offset + width > tex_width)
		{
			w_offset = 0;
			h_offset += max_height;
		}

		// Copy bitmap data to texture
		if (bitmap.buffer)
		{
			const unsigned char *buf = bitmap.buffer;
			unsigned char *d_ptr = tex_buf.get()
			                     + (h_offset * tex_width + w_offset) * 2
			                     + (kGlyphPadding * 2) * (tex_width + 1);
			uint_t tmp = (tex_width - bitmap.width) * 2;
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
		fg.advance = static_cast<short>(slot->advance.x / 64);
		fg.left = slot->bitmap_left;

	#ifdef FONTATLAS_KERNING

		if (FT_HAS_KERNING(face))
		{
			uint_t glyph = i + kASCIIOffset;
			FT_Vector kerning;
			for (uint_t j = 0; j < kNumCharacters; ++j)
			{
				FT_Get_Kerning(face, j + kASCIIOffset, glyph, FT_KERNING_DEFAULT, &kerning);
				fg.kern[j] = static_cast<short>(kerning.x / 64);
			}
		}

	#endif

		fg.quad[0].position.x = -kGlyphMargin;
		fg.quad[0].position.y = static_cast<float>(slot->bitmap_top - bitmap.rows) - kGlyphMargin;
		fg.quad[1].position.x = static_cast<float>(bitmap.width) + kGlyphMargin;
		fg.quad[1].position.y = fg.quad[0].position.y;
		fg.quad[2].position.x = fg.quad[1].position.x;
		fg.quad[2].position.y = static_cast<float>(slot->bitmap_top) + kGlyphMargin;
		fg.quad[3].position.x = fg.quad[0].position.x;
		fg.quad[3].position.y = fg.quad[2].position.y;

		fg.quad[0].texcoord.x = (kGlyphPadding - kGlyphMargin + w_offset) * tex_w_fraction;
		fg.quad[0].texcoord.y = (kGlyphPadding + bitmap.rows + kGlyphMargin + h_offset) * tex_h_fraction;
		fg.quad[1].texcoord.x = (kGlyphPadding + bitmap.width + kGlyphMargin + w_offset) * tex_w_fraction;
		fg.quad[1].texcoord.y = fg.quad[0].texcoord.y;
		fg.quad[2].texcoord.x = fg.quad[1].texcoord.x;
		fg.quad[2].texcoord.y = (kGlyphPadding - kGlyphMargin + h_offset) * tex_h_fraction;
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
	const GLubyte *i = tex_buf.get();
	for (uint_t y = 0; y < tex_height; ++y)
	{
		for (uint_t x = 0; x < tex_width; ++x)
		{
			printf("%c", (*i > 0) ? 'X' : ' ');
			i += 2;
		}
		puts("");
	}
	 */

	this->texture = TextureManager::Instance->create(
			GL_LUMINANCE_ALPHA, tex_width, tex_height,
			GL_LUMINANCE_ALPHA, tex_buf.get());
}

const FontGlyph* FontAtlas::get_glyph(const unsigned int c) const
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
