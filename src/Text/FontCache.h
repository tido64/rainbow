// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef TEXT_FONTCACHE_H_
#define TEXT_FONTCACHE_H_

#include <array>
#include <string>

#include "ThirdParty/DisableWarnings.h"
#include <absl/container/flat_hash_map.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ThirdParty/ReenableWarnings.h"

#include <imgui/imstb_rectpack.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Common/Global.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Texture.h"
#include "Graphics/TextureManager.h"
#include "Memory/ArrayMap.h"

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

        template <typename H>
        friend auto AbslHashValue(H hash_state, const GlyphCacheIndex& i) -> H
        {
            return H::combine(
                std::move(hash_state), i.face, i.font_size, i.index);
        }

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
        absl::flat_hash_map<GlyphCacheIndex, GlyphInfo> glyph_cache_;
        ArrayMap<std::string, FontFace> font_cache_;
        stbrp_context bin_context_;
        std::array<stbrp_node, kTextureSize> bin_nodes_;
        std::unique_ptr<uint8_t[]> bitmap_;
        FT_Library library_;
    };
}  // namespace rainbow

#endif
