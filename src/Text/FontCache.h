// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef TEXT_FONTCACHE_H_
#define TEXT_FONTCACHE_H_

#include <array>
#include <string>

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <absl/container/flat_hash_map.h>  // NOLINT(llvm-include-order)
#include <ft2build.h>
#include FT_FREETYPE_H
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include <imgui/imstb_rectpack.h>

#include "Common/Data.h"
#include "Common/Global.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Texture.h"
#include "Memory/ArrayMap.h"

namespace rainbow
{
    class FontCache : public Global<FontCache>
    {
    public:
        static constexpr auto kTextureSize = 1024;

        FontCache();
        ~FontCache();

        [[nodiscard]] auto texture() const -> const graphics::Texture&
        {
            return texture_;
        }

        auto get(std::string_view font_name) -> FT_Face;
        auto get_glyph(FT_Face face, int32_t font_size, uint32_t glyph_index)
            -> std::array<SpriteVertex, 4>;

        void update(graphics::TextureProvider&);

    private:
        struct FontFace {
            FT_Face face;
            Data data;
        };

        struct GlyphInfo {
            std::array<SpriteVertex, 4> vertices;
        };

        struct Index {
            FT_Face face;
            int32_t font_size;
            uint32_t index;

            template <typename H>
            friend auto AbslHashValue(H hash_state, const Index& i) -> H
            {
                return H::combine(
                    std::move(hash_state), i.face, i.font_size, i.index);
            }

            friend auto operator==(const Index& lhs, const Index& rhs) -> bool
            {
                return lhs.face == rhs.face && lhs.font_size == rhs.font_size &&
                       lhs.index == rhs.index;
            }
        };

        enum class State {
            Ready,
            NeedsUpdate,
        };

        State state_ = State::Ready;
        graphics::Texture texture_;
        absl::flat_hash_map<Index, GlyphInfo> glyph_cache_;
        ArrayMap<std::string, FontFace> font_cache_;
        stbrp_context bin_context_;
        std::array<stbrp_node, kTextureSize> bin_nodes_;
        std::unique_ptr<uint8_t[]> bitmap_;
        FT_Library library_;
    };
}  // namespace rainbow

#endif
