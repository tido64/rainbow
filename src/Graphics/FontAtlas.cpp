// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/FontAtlas.h"

#include <algorithm>
#include <memory>

#include <ft2build.h>
#include FT_CFF_DRIVER_H
#include FT_GLYPH_H
#include FT_MODULE_H

#include "Common/Data.h"
#include "Graphics/OpenGL.h"
#include "Graphics/TextureManager.h"

#ifdef _MSC_VER
#	undef max  // TODO: Interferes with std::numeric_limits::max().
#endif

namespace
{
	using uchar_t = unsigned char;
	using uint_t = unsigned int;

	const float kGlyphMargin = 2.0f;     ///< Margin around rendered font glyph.
	const uint_t kASCIIOffset = 32;      ///< Start loading from character 32.
	const uint_t kDPI = 96;              ///< Horizontal/vertical resolution in dpi.
	const uint_t kGlyphPadding = 3;      ///< Padding around font glyph texture.
	const uint_t kGlyphPadding2 = kGlyphPadding * 2;
	const int kNumGlyphsPerColRow = 12;  ///< Number of glyphs per column/row on texture.
	const int kPixelFormat = 64;         ///< 26.6 fixed-point pixel coordinates.

	void copy_bitmap_into(uchar_t *dst, const Vec2i &dst_sz, const Vec2i &off,
	                      const uchar_t *src, const Vec2i &src_sz)
	{
		const size_t stride = (dst_sz.width - src_sz.width) * 2;
		size_t i = 0;
		uchar_t *ptr = dst + (off.y * dst_sz.width + off.x) * 2;
		for (int y = 0; y < src_sz.height; ++y)
		{
			for (int x = 0; x < src_sz.width; ++x)
			{
				*ptr = std::numeric_limits<uchar_t>::max();
				*(++ptr) = src[i];
				++ptr;
				++i;
			}
			ptr += stride;
		}
	}

	Vec2i max_glyph_size(FT_Face face, FontGlyph *glyphs, const size_t count)
	{
		Vec2i max;
		std::for_each(glyphs,
		              glyphs + count,
		              [&, face](const FontGlyph &glyph) {
			if (FT_Load_Char(face, glyph.code, FT_LOAD_DEFAULT))
			{
				R_ASSERT(false, "Failed to load characters");
				max.width = -1;
				return;
			}
			const FT_Glyph_Metrics &metrics = face->glyph->metrics;
			if (metrics.width > max.width)
				max.width = metrics.width;
			if (metrics.height > max.height)
				max.height = metrics.height;
		});
		if (max.width < 0)
			return Vec2i();
		return Vec2i(max.width / kPixelFormat + kGlyphPadding2,
		             max.height / kPixelFormat + kGlyphPadding2);
	}

	class FontFace
	{
	public:
		FontFace(FT_Library library, const Data &font) : face_(nullptr)
		{
			FT_Error error =
			    FT_New_Memory_Face(
			        library, static_cast<const FT_Byte*>(font.bytes()),
			        font.size(), 0, &face_);
			R_ASSERT(!error, "Failed to load font face");
			R_ASSERT(FT_IS_SCALABLE(face_),
			         "Unscalable fonts are not supported");
			error = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
			R_ASSERT(!error, "Failed to select character map");
		}

		~FontFace()
		{
			if (!face_)
				return;

			FT_Done_Face(face_);
		}

		FT_Face operator->() const
		{
			return face_;
		}

		explicit operator bool() const
		{
			return face_ != nullptr;
		}

		operator FT_Face() const
		{
			return face_;
		}

	private:
		FT_Face face_;
	};

	class FontLibrary
	{
	public:
		FontLibrary() : library_(nullptr)
		{
			FT_Init_FreeType(&library_);
			R_ASSERT(library_, "Failed to initialise FreeType");
		}

		~FontLibrary()
		{
			if (!library_)
				return;

			FT_Done_FreeType(library_);
		}

		explicit operator bool() const
		{
			return library_ != nullptr;
		}

		operator FT_Library() const
		{
			return library_;
		}

	private:
		FT_Library library_;
	};
}

FontAtlas::FontAtlas(const Data &font, const float pt)
    : pt_(pt), height_(0), texture_(0)
{
	R_ASSERT(font, "Failed to load font");

	for (uint_t i = 0; i < kNumCharacters; ++i)
		charset_[i].code = i + kASCIIOffset;

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
		charset_[kNumCharacters + i].code = characters[i];
#endif

	FontLibrary library;
	if (!library)
		return;

	FontFace face(library, font);
	if (!face)
		return;

	FT_Set_Char_Size(face, 0, pt_ * kPixelFormat, kDPI, kDPI);
	const Vec2i &max =
	    max_glyph_size(face, charset_, sizeof(charset_) / sizeof(charset_[0]));
	if (max.is_zero())
		return;
	const Vec2i size(Rainbow::next_pow2(max.width * kNumGlyphsPerColRow),
	                 Rainbow::next_pow2(max.height * kNumGlyphsPerColRow));

	// GL_LUMINANCE8_ALPHA8 buffer
	Vec2i offset(size.width * size.height * 2, 0);
	std::unique_ptr<GLubyte[]> buffer(new GLubyte[offset.x]);
	std::fill_n(buffer.get(), offset.x, 0);

	// Copy all glyph bitmaps to our texture.
	offset.x = 0;
	const Vec2f pixel(1.0f / size.width, 1.0f / size.height);
	for (auto &glyph : charset_)
	{
		FT_Load_Char(face, glyph.code, FT_LOAD_RENDER);

		const FT_GlyphSlot &slot = face->glyph;
		const FT_Bitmap &bitmap = slot->bitmap;

		// Make sure bitmap data has enough space to the right. Otherwise, start
		// a new line.
		const int width = bitmap.width + kGlyphPadding2;
		if (offset.x + width > size.width)
		{
			offset.x = 0;
			offset.y += max.height;
		}

		// Copy bitmap data to texture.
		if (bitmap.buffer)
		{
			copy_bitmap_into(buffer.get(), size, offset + kGlyphPadding,
			                 bitmap.buffer, Vec2i(bitmap.width, bitmap.rows));
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
			for (auto &left : charset_)
			{
				FT_Get_Kerning(face, left.code, glyph.code, FT_KERNING_DEFAULT, &kerning);
				glyph.kern[++i] = static_cast<short>(kerning.x / kPixelFormat);
			}
		}
	#endif

		// Advance to the next "slot" in our texture.
		offset.x += width;
	}
	height_ = face->size->metrics.height / kPixelFormat;

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

	texture_ =
	    TextureManager::Instance->create(
	        GL_LUMINANCE_ALPHA, size.width, size.height, GL_LUMINANCE_ALPHA,
	        buffer.get());
}

FontAtlas::~FontAtlas()
{
	TextureManager::Instance->remove(texture_);
}

void FontAtlas::bind() const
{
	TextureManager::Instance->bind(texture_);
}

const FontGlyph* FontAtlas::get_glyph(const uint_t c) const
{
#if FONTATLAS_EXTENDED > 0
	if (c >= 0x80u)
	{
		const auto first = charset_ + kNumCharacters;
		const auto last = first + FONTATLAS_EXTENDED;
		const auto i = std::find_if(first, last, [c] (const FontGlyph &glyph) {
			return glyph.code == c;
		});
		return (i == last) ? nullptr : i;
	}
#endif
	return &charset_[static_cast<uchar_t>(c) - kASCIIOffset];
}
