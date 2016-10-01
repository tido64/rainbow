// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/FontAtlas.h"

#include <memory>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <ft2build.h>
#include FT_CFF_DRIVER_H
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "Common/Data.h"
#include "Graphics/OpenGL.h"
#include "Graphics/TextureManager.h"

using rainbow::Texture;
using rainbow::array_size;

using uchar_t = unsigned char;

namespace
{
    constexpr float kGlyphMargin = 2.0f;          ///< Margin around rendered font glyph.
    constexpr uint32_t kASCIIOffset = 32;         ///< Start loading from character 32.
    constexpr uint32_t kDPI = 96;                 ///< Horizontal/vertical resolution in dpi.
    constexpr uint32_t kGlyphPadding = 3;         ///< Padding around font glyph texture.
    constexpr uint32_t kGlyphPadding2 = kGlyphPadding * 2;
    constexpr uint32_t kNumGlyphsPerColRow = 12;  ///< Number of glyphs per column/row on texture.
    constexpr int kPixelFormat = 64;              ///< 26.6 fixed-point pixel coordinates.

    void copy_bitmap_into(uchar_t* dst,
                          const Vec2u& dst_sz,
                          const Vec2u& off,
                          const uchar_t* src,
                          const Vec2u& src_sz)
    {
        const size_t stride = (dst_sz.x - src_sz.x) * 2;
        size_t i = 0;
        uchar_t* ptr = dst + (off.y * dst_sz.x + off.x) * 2;
        for (uint32_t y = 0; y < src_sz.y; ++y)
        {
            for (uint32_t x = 0; x < src_sz.x; ++x)
            {
                *ptr = std::numeric_limits<uchar_t>::max();
                *(++ptr) = src[i];
                ++ptr;
                ++i;
            }
            ptr += stride;
        }
    }

    Vec2u max_glyph_size(const FT_Face& face,
                         const FontGlyph* glyphs,
                         size_t count)
    {
        Vec2u max;
        std::for_each(glyphs,
                      glyphs + count,
                      [&face, &max](const FontGlyph& glyph) {
            if (FT_Load_Char(face, glyph.code, FT_LOAD_DEFAULT))
            {
                max.x = 0;
                R_ABORT("Failed to load characters");
                UNREACHABLE();
                return;
            }
            const FT_Glyph_Metrics& metrics = face->glyph->metrics;
            if (metrics.width > static_cast<FT_Pos>(max.x))
                max.x = static_cast<Vec2u::value_type>(metrics.width);
            if (metrics.height > static_cast<FT_Pos>(max.y))
                max.y = static_cast<Vec2u::value_type>(metrics.height);
        });
        if (max.x == 0)
            return Vec2u();
        return Vec2u(max.x / kPixelFormat + kGlyphPadding2,
                     max.y / kPixelFormat + kGlyphPadding2);
    }

    class FontFace
    {
    public:
        FontFace(FT_Library library, const Data& font) : face_(nullptr)
        {
            FT_Error error =
                FT_New_Memory_Face(library,
                                   static_cast<const FT_Byte*>(font.bytes()),
                                   static_cast<FT_Long>(font.size()),
                                   0,
                                   &face_);
            R_ASSERT(!error, "Failed to load font face");
            R_ASSERT(FT_IS_SCALABLE(face_),
                     "Unscalable fonts are not supported");
            error = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
            R_ASSERT(!error, "Failed to select character map");
            NOT_USED(error);
        }

        ~FontFace()
        {
            if (!face_)
                return;

            FT_Done_Face(face_);
        }

        FT_Face operator->() const { return face_; }
        explicit operator bool() const { return face_ != nullptr; }
        operator const FT_Face&() const { return face_; }

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

        explicit operator bool() const { return library_ != nullptr; }
        operator const FT_Library&() const { return library_; }

    private:
        FT_Library library_;
    };
}

FontAtlas::FontAtlas(const char* path, float pt)
    : FontAtlas(path, Data::load_asset(path), pt) {}

FontAtlas::FontAtlas(const char* name, const Data& font, float pt)
    : pt_(pt), height_(0)
{
    texture_ = TextureManager::Get()->create(
        name,
        [this, &font](TextureManager& texture_manager, const Texture& texture)
        {
            load(texture_manager, texture, font);
        });
}

auto FontAtlas::get_glyph(uint32_t c) const -> const FontGlyph*
{
#if FONTATLAS_EXTENDED > 0
    if (c >= 0x80u)
    {
        const auto first = charset_ + kNumCharacters;
        const auto last = first + FONTATLAS_EXTENDED;
        const auto i = std::find_if(first, last, [c](const FontGlyph& glyph) {
            return glyph.code == c;
        });
        return (i == last ? nullptr : i);
    }
#endif
    return &charset_[static_cast<uchar_t>(c) - kASCIIOffset];
}

void FontAtlas::load(TextureManager& texture_manager,
                     const rainbow::Texture& texture,
                     const Data& font)
{
    R_ASSERT(font, "Failed to load font");

    for (uint32_t i = 0; i < kNumCharacters; ++i)
        charset_[i].code = i + kASCIIOffset;

#if FONTATLAS_EXTENDED > 0
    const uint32_t characters[]{
        0x00c5,  // LATIN CAPITAL LETTER A WITH RING ABOVE
        0x00c6,  // LATIN CAPITAL LETTER AE
        0x00d8,  // LATIN CAPITAL LETTER O WITH STROKE
        0x00e5,  // LATIN SMALL LETTER A WITH RING ABOVE
        0x00e6,  // LATIN SMALL LETTER AE
        0x00f8,  // LATIN SMALL LETTER O WITH STROKE
    };
    for (size_t i = 0; i < array_size(characters); ++i)
        charset_[kNumCharacters + i].code = characters[i];
#endif

    FontLibrary library;
    if (!library)
        return;

    FontFace face(library, font);
    if (!face)
        return;

    FT_Set_Char_Size(face, 0, pt_ * kPixelFormat, kDPI, kDPI);
    const Vec2u& max = max_glyph_size(face, charset_, array_size(charset_));
    if (max.is_zero())
        return;
    const Vec2u size(rainbow::ceil_pow2(max.x * kNumGlyphsPerColRow),
                     rainbow::ceil_pow2(max.y * kNumGlyphsPerColRow));

    // GL_LUMINANCE8_ALPHA8 buffer
    Vec2u offset(size.x * size.y * 2, 0);
    auto buffer = std::make_unique<GLubyte[]>(offset.x);
    std::fill_n(buffer.get(), offset.x, 0);

    // Copy all glyph bitmaps to our texture.
    offset.x = 0;
    const Vec2f pixel(1.0f / size.x, 1.0f / size.y);
    for (auto& glyph : charset_)
    {
        FT_Load_Char(face, glyph.code, FT_LOAD_RENDER);

        const FT_GlyphSlot& slot = face->glyph;
        const FT_Bitmap& bitmap = slot->bitmap;

        // Make sure bitmap data has enough space to the right. Otherwise, start
        // a new line.
        const uint32_t width = bitmap.width + kGlyphPadding2;
        if (offset.x + width > size.x)
        {
            offset.x = 0;
            offset.y += max.y;
        }

        // Copy bitmap data to texture.
        if (bitmap.buffer)
        {
            R_ASSERT(bitmap.num_grays == 256, "");
            R_ASSERT(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY, "");
            copy_bitmap_into(buffer.get(),
                             size,
                             offset + kGlyphPadding,
                             bitmap.buffer,
                             Vec2u(bitmap.width, bitmap.rows));
        }

        // Save font glyph data.
        glyph.advance = slot->advance.x / kPixelFormat;
        glyph.left = slot->bitmap_left;

        SpriteVertex* vx = glyph.quad;

        vx[0].position.x = -kGlyphMargin;
        vx[0].position.y = static_cast<float>(slot->bitmap_top -
                                              static_cast<int>(bitmap.rows)) -
                           kGlyphMargin;
        vx[1].position.x = static_cast<float>(bitmap.width) + kGlyphMargin;
        vx[1].position.y = vx[0].position.y;
        vx[2].position.x = vx[1].position.x;
        vx[2].position.y = static_cast<float>(slot->bitmap_top) + kGlyphMargin;
        vx[3].position.x = vx[0].position.x;
        vx[3].position.y = vx[2].position.y;

        vx[0].texcoord.x = (kGlyphPadding - kGlyphMargin + offset.x) * pixel.x;
        vx[0].texcoord.y =
            (kGlyphPadding + bitmap.rows + kGlyphMargin + offset.y) * pixel.y;
        vx[1].texcoord.x =
            (kGlyphPadding + bitmap.width + kGlyphMargin + offset.x) * pixel.x;
        vx[1].texcoord.y = vx[0].texcoord.y;
        vx[2].texcoord.x = vx[1].texcoord.x;
        vx[2].texcoord.y = (kGlyphPadding - kGlyphMargin + offset.y) * pixel.y;
        vx[3].texcoord.x = vx[0].texcoord.x;
        vx[3].texcoord.y = vx[2].texcoord.y;

#ifdef FONTATLAS_KERNING
        if (FT_HAS_KERNING(face))
        {
            int i = -1;
            FT_Vector kerning;
            for (auto& left : charset_)
            {
                FT_Get_Kerning(
                    face, left.code, glyph.code, FT_KERNING_DEFAULT, &kerning);
                glyph.kern[++i] = static_cast<short>(kerning.x / kPixelFormat);
            }
        }
#endif

        // Advance to the next "slot" in our texture.
        offset.x += width;
    }

    height_ = face->size->metrics.height / kPixelFormat;

    texture_manager.upload(texture, GL_LUMINANCE_ALPHA, size.x, size.y,
                           GL_LUMINANCE_ALPHA, buffer.get());
}
