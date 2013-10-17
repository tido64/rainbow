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

	const float kGlyphMargin = 2.0f;     ///< Margin around rendered font glyph.
	const uint_t kASCIIOffset = 32;      ///< Start loading from character 32.
	const uint_t kDPI = 96;              ///< Horizontal/vertical resolution in dpi.
	const uint_t kGlyphPadding = 3;      ///< Padding around font glyph texture.
	const int kNumGlyphsPerColRow = 12;  ///< Number of glyphs per column/row on texture.
	const int kPixelFormat = 64;         ///< 26.6 fixed-point pixel coordinates.

	class FontFace
	{
	public:
		FontFace(FT_Library library, const Data &font) : face(nullptr)
		{
			FT_Error error = FT_New_Memory_Face(library,
			                                    static_cast<const FT_Byte*>(font.bytes()),
			                                    font.size(),
			                                    0,
			                                    &this->face);
			R_ASSERT(!error, "Failed to load font face");
			R_ASSERT(FT_IS_SCALABLE(this->face), "Unscalable fonts are not supported");
			error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);
			R_ASSERT(!error, "Failed to select character map");
		}

		~FontFace()
		{
			if (!this->face)
				return;

			FT_Done_Face(this->face);
		}

		FT_Face operator->() const
		{
			return this->face;
		}

		operator bool() const
		{
			return this->face != nullptr;
		}

		operator FT_Face() const
		{
			return this->face;
		}

	private:
		FT_Face face;
	};

	class FontLibrary
	{
	public:
		FontLibrary() : library(nullptr)
		{
			FT_Init_FreeType(&this->library);
			R_ASSERT(library, "Failed to initialise FreeType");
		}

		~FontLibrary()
		{
			if (!this->library)
				return;

			FT_Done_FreeType(this->library);
		}

		operator bool() const
		{
			return this->library != nullptr;
		}

		operator FT_Library() const
		{
			return this->library;
		}

	private:
		FT_Library library;
	};
}

FontAtlas::FontAtlas(const Data &font, const float pt) : pt_(pt), height_(0), texture_(0)
{
	R_ASSERT(font, "Failed to load font");

	for (uint_t i = 0; i < kNumCharacters; ++i)
		this->charset_[i].code = i + kASCIIOffset;

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
	for (uint_t i = 0; characters[i]; ++i)
		this->charset_[kNumCharacters + i].code = characters[i];
#endif

	FontLibrary library;
	if (!library)
		return;

	FontFace face(library, font);
	if (!face)
		return;

	FT_Set_Char_Size(face, 0, this->pt_ * kPixelFormat, kDPI, kDPI);
	const int padding = kGlyphPadding * 2;

	// Naive algorithm for calculating texture size.
	int max_width = 0;
	int max_height = 0;
	for (const auto &glyph : this->charset_)
	{
		if (FT_Load_Char(face, glyph.code, FT_LOAD_DEFAULT))
		{
			R_ASSERT(false, "Failed to load characters");
			return;
		}
		const FT_Glyph_Metrics &metrics = face->glyph->metrics;
		if (metrics.width > max_width)
			max_width = metrics.width;
		if (metrics.height > max_height)
			max_height = metrics.height;
	}
	max_width = max_width / kPixelFormat + padding;
	max_height = max_height / kPixelFormat + padding;
	const Vec2i size(Rainbow::next_pow2(max_width * kNumGlyphsPerColRow),
	                 Rainbow::next_pow2(max_height * kNumGlyphsPerColRow));

	// GL_LUMINANCE8_ALPHA8 buffer
	Vec2i offset(size.width * size.height * 2, 0);
	std::unique_ptr<GLubyte[]> buffer(new GLubyte[offset.x]);
	memset(buffer.get(), 0, offset.x);

	// Copy all glyph bitmaps to our texture.
	offset.x = 0;
	const Vec2f pixel(1.0f / size.width, 1.0f / size.height);
	for (auto &glyph : this->charset_)
	{
		FT_Load_Char(face, glyph.code, FT_LOAD_RENDER);

		const FT_GlyphSlot &slot = face->glyph;
		const FT_Bitmap &bitmap = slot->bitmap;

		// Make sure bitmap data has enough space to the right. Otherwise, start
		// a new line.
		const int width = bitmap.width + padding;
		if (offset.x + width > size.width)
		{
			offset.x = 0;
			offset.y += max_height;
		}

		// Copy bitmap data to texture.
		if (bitmap.buffer)
		{
			const unsigned char *bit = bitmap.buffer;
			unsigned char *buf = buffer.get()
			                   + (offset.y * size.width + offset.x) * 2
			                   + padding * (size.width + 1);
			const uint_t stride = (size.width - bitmap.width) * 2;
			for (int y = 0; y < bitmap.rows; ++y)
			{
				for (int x = 0; x < bitmap.width; ++x)
				{
				#ifdef RAINBOW_IOS
					*buf = *(buf + 1) = *bit;
				#else
					if (*bit)
					{
						*buf = 255;
						*(buf + 1) = *bit;
					}
				#endif
					buf += 2;
					++bit;
				}
				buf += stride;
			}
		}

		// Save font glyph data.
		glyph.advance = slot->advance.x / kPixelFormat;
		glyph.left = slot->bitmap_left;

		SpriteVertex *vx = glyph.quad;

		vx[0].position.x = -kGlyphMargin;
		vx[0].position.y = static_cast<float>(slot->bitmap_top - bitmap.rows) - kGlyphMargin;
		vx[1].position.x = static_cast<float>(bitmap.width) + kGlyphMargin;
		vx[1].position.y = vx[0].position.y;
		vx[2].position.x = vx[1].position.x;
		vx[2].position.y = static_cast<float>(slot->bitmap_top) + kGlyphMargin;
		vx[3].position.x = vx[0].position.x;
		vx[3].position.y = vx[2].position.y;

		vx[0].texcoord.x = (kGlyphPadding - kGlyphMargin + offset.x) * pixel.width;
		vx[0].texcoord.y = (kGlyphPadding + bitmap.rows + kGlyphMargin + offset.y) * pixel.height;
		vx[1].texcoord.x = (kGlyphPadding + bitmap.width + kGlyphMargin + offset.x) * pixel.width;
		vx[1].texcoord.y = vx[0].texcoord.y;
		vx[2].texcoord.x = vx[1].texcoord.x;
		vx[2].texcoord.y = (kGlyphPadding - kGlyphMargin + offset.y) * pixel.height;
		vx[3].texcoord.x = vx[0].texcoord.x;
		vx[3].texcoord.y = vx[2].texcoord.y;

	#ifdef FONTATLAS_KERNING
		if (FT_HAS_KERNING(face))
		{
			int i = -1;
			FT_Vector kerning;
			for (auto &left : this->charset_)
			{
				FT_Get_Kerning(face, left.code, glyph.code, FT_KERNING_DEFAULT, &kerning);
				glyph.kern[++i] = static_cast<short>(kerning.x / kPixelFormat);
			}
		}
	#endif

		// Advance to the next "slot" in our texture.
		offset.x += width;
	}
	this->height_ = face->size->metrics.height / kPixelFormat;

	/**
	 * For printing out texture buffer
	 *
	const GLubyte *i = buffer.get();
	for (int y = 0; y < size.height; ++y)
	{
		for (int x = 0; x < size.width; ++x)
		{
			printf("%c", (*i > 0) ? 'X' : ' ');
			i += 2;
		}
		puts("");
	}
	 */

	this->texture_ = TextureManager::Instance->create(
			GL_LUMINANCE_ALPHA, size.width, size.height,
			GL_LUMINANCE_ALPHA, buffer.get());
}

const FontGlyph* FontAtlas::get_glyph(const unsigned int c) const
{
#if FONTATLAS_EXTENDED > 0
	if (c >= 0x80u)
	{
		for (size_t i = kNumCharacters; i < kNumCharacters + FONTATLAS_EXTENDED; ++i)
			if (this->charset_[i].code == c)
				return &this->charset_[i];
		return nullptr;
	}
#endif
	return &this->charset_[static_cast<unsigned char>(c) - kASCIIOffset];
}
