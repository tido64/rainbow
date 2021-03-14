// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Text/FontCache.h"

#include "Common/Logging.h"
#include "Common/TypeCast.h"
#include "FileSystem/File.h"
#include "Graphics/Image.h"
#include "Text/SystemFonts.h"

using rainbow::Color;
using rainbow::FontCache;
using rainbow::SpriteVertex;
using rainbow::Vec2i;
using rainbow::graphics::TextureProvider;

namespace
{
    /// <summary>Horizontal/vertical resolution in dpi.</summary>
    constexpr uint32_t kDPI = 96;

    constexpr int kGlyphMargin = 1;

    /// <summary>26.6 fixed-point pixel coordinates.</summary>
    constexpr int kPixelFormat = 64;

    constexpr size_t kTextureSizeBytes =
        FontCache::kTextureSize * FontCache::kTextureSize * 4;

    void blit(const uint8_t* src,
              const stbrp_rect& src_rect,
              Color* dst,
              const Vec2i& dst_sz)
    {
        for (uint32_t row = 0; row < src_rect.h; ++row) {
            auto out = dst + ((src_rect.y + row) * dst_sz.x + src_rect.x);
            const auto begin = src + src_rect.w * row;
            std::transform(begin, begin + src_rect.w, out, [](uint8_t alpha) {
                // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
                return Color{0xff, alpha};
            });
        }
    }
}  // namespace

FontCache::FontCache()
{
    const auto texture_size = ceil_pow2(kTextureSize);
    stbrp_init_target(&bin_context_,
                      texture_size,
                      texture_size,
                      bin_nodes_.data(),
                      narrow_cast<int>(bin_nodes_.size()));

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
    bitmap_ = std::make_unique<uint8_t[]>(kTextureSizeBytes);
    std::fill_n(bitmap_.get(), kTextureSizeBytes, 0);

    FT_Init_FreeType(&library_);
    R_ASSERT(library_, "Failed to initialise FreeType");

    make_global();
}

FontCache::~FontCache()
{
    if (library_ == nullptr)
        return;

    for (auto&& i : font_cache_)
        FT_Done_Face(i.second.face);

    FT_Done_FreeType(library_);
}

auto FontCache::get(std::string_view font_name) -> FT_Face
{
    auto search = font_cache_.find(font_name);
    if (search == font_cache_.end()) {
        auto data = font_name.empty()
                        ? text::monospace_font()
                        : File::read(font_name.data(), FileType::Asset);
        FT_Face face;
        [[maybe_unused]] FT_Error error =
            FT_New_Memory_Face(library_,
                               data.as<FT_Byte*>(),
                               narrow_cast<FT_Long>(data.size()),
                               0,
                               &face);

        R_ASSERT(error == FT_Err_Ok, "Failed to load font face");
        R_ASSERT(FT_IS_SCALABLE(face), "Unscalable fonts are not supported");

        error = FT_Select_Charmap(face, FT_ENCODING_UNICODE);

        R_ASSERT(error == FT_Err_Ok, "Failed to select character map");

        font_cache_.emplace(font_name, FontFace{face, std::move(data)});
        return face;
    }

    return search->second.face;
}

auto FontCache::get_glyph(FT_Face face, int32_t font_size, uint32_t glyph_index)
    -> std::array<SpriteVertex, 4>
{
    const Index cache_index{face, font_size, glyph_index};
    auto search = glyph_cache_.find(cache_index);
    if (search == glyph_cache_.end()) {
        FT_Set_Char_Size(face, 0, font_size * kPixelFormat, 0, kDPI);
        FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
        FT_GlyphSlot slot = face->glyph;
        const FT_Bitmap& bitmap = slot->bitmap;

        R_ASSERT(bitmap.num_grays == 256, "");
        R_ASSERT(bitmap.pixel_mode == FT_PIXEL_MODE_GRAY, "");

        stbrp_rect rect{
            0,
            static_cast<stbrp_coord>(bitmap.width + kGlyphMargin * 2),
            static_cast<stbrp_coord>(bitmap.rows + kGlyphMargin * 2),
            0,
            0,
            0};
        stbrp_pack_rects(&bin_context_, &rect, 1);

        R_ASSERT(rect.was_packed != 0,
                 "Failed to pack a glyph, please increase your texture size");

        // Adjust coordinates to compensate for margins.
        rect.w -= kGlyphMargin * 2;
        rect.h -= kGlyphMargin * 2;
        rect.x += kGlyphMargin;
        rect.y += kGlyphMargin;

        blit(bitmap.buffer,
             rect,
             reinterpret_cast<Color*>(bitmap_.get()),
             {kTextureSize, kTextureSize});
        state_ = State::NeedsUpdate;

        std::array<SpriteVertex, 4> vx;

        vx[0].position.x = slot->bitmap_left;
        vx[0].position.y = narrow_cast<float>(slot->bitmap_top -
                                              narrow_cast<FT_Int>(bitmap.rows));
        vx[1].position.x = narrow_cast<float>(slot->bitmap_left + bitmap.width);
        vx[1].position.y = vx[0].position.y;
        vx[2].position.x = vx[1].position.x;
        vx[2].position.y = narrow_cast<float>(slot->bitmap_top);
        vx[3].position.x = vx[0].position.x;
        vx[3].position.y = vx[2].position.y;

        vx[0].texcoord.x = rect.x / narrow_cast<float>(kTextureSize);
        vx[0].texcoord.y = (rect.y + rect.h) / narrow_cast<float>(kTextureSize);
        vx[1].texcoord.x = (rect.x + rect.w) / narrow_cast<float>(kTextureSize);
        vx[1].texcoord.y = vx[0].texcoord.y;
        vx[2].texcoord.x = vx[1].texcoord.x;
        vx[2].texcoord.y = rect.y / narrow_cast<float>(kTextureSize);
        vx[3].texcoord.x = vx[0].texcoord.x;
        vx[3].texcoord.y = vx[2].texcoord.y;

        glyph_cache_[cache_index] = {vx};
        return vx;
    }

    return search->second.vertices;
}

void FontCache::update(TextureProvider& texture_provider)
{
    if (state_ != State::Ready) {
        const auto image = Image{
            Image::Format::RGBA,
            narrow_cast<uint32_t>(kTextureSize),
            narrow_cast<uint32_t>(kTextureSize),
            32U,
            4U,
            kTextureSizeBytes,
            bitmap_.get(),
        };
        if (!texture_)
            texture_ = texture_provider.get("rainbow://font-cache", image);
        else
            texture_provider.update(texture_, image);
        state_ = State::Ready;
    }
}

#define STB_RECT_PACK_IMPLEMENTATION
// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <imgui/imstb_rectpack.h>  // NOLINT(llvm-include-order)
#include "ThirdParty/ReenableWarnings.h"
// clang-format on
