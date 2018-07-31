// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef TEXT_FONTCACHE_H_
#define TEXT_FONTCACHE_H_

#include <array>
#include <string>
#include <unordered_map>

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wold-style-cast"
#endif
#include <ft2build.h>
#include FT_FREETYPE_H
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include <stb/stb_rect_pack.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Common/Global.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"

namespace rainbow
{
    struct FontFace
    {
        FT_Face face;
        Data data;
    };

    struct GlyphCacheIndex
    {
        FT_Face face;
        int32_t font_size;
        uint32_t index;

        friend bool operator==(const GlyphCacheIndex& lhs,
                               const GlyphCacheIndex& rhs)
        {
            return lhs.face == rhs.face && lhs.font_size == rhs.font_size &&
                   lhs.index == rhs.index;
        }
    };

    struct GlyphInfo
    {
        std::array<SpriteVertex, 4> vertices;
    };
}  // namespace rainbow

namespace std
{
    template <>
    struct hash<rainbow::GlyphCacheIndex>
    {
        auto operator()(const rainbow::GlyphCacheIndex& glyph) const -> size_t
        {
            size_t seed = 0;
            rainbow::hash_combine(seed, glyph.face);
            rainbow::hash_combine(seed, glyph.font_size);
            rainbow::hash_combine(seed, glyph.index);
            return seed;
        }
    };
}  // namespace std

namespace rainbow
{
    class FontCache : public Global<FontCache>
    {
    public:
        static constexpr int kTextureSize = 1024;

        FontCache();
        ~FontCache();

        void bind_texture() const { texture_.bind(); }

        auto get(std::string_view font_name) -> FT_Face;
        auto get_glyph(FT_Face face, int32_t font_size, uint32_t glyph_index)
            -> std::array<SpriteVertex, 4>;

        void update(graphics::TextureManager& texture_manager);

    private:
        bool is_stale_;
        graphics::Texture texture_;
        std::unordered_map<GlyphCacheIndex, GlyphInfo> glyph_cache_;
        std::unordered_map<std::string, FontFace> font_cache_;
        stbrp_context bin_context_;
        std::array<stbrp_node, kTextureSize> bin_nodes_;
        std::unique_ptr<uint8_t[]> bitmap_;
        FT_Library library_;
    };
}  // namespace rainbow

#endif
