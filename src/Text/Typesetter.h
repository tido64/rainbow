// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef TEXT_TYPESETTER_H_
#define TEXT_TYPESETTER_H_

#include <string>
#include <string_view>
#include <vector>

#include <Rainbow/TextAlignment.h>

#include "Common/Global.h"
#include "Math/Vec2.h"
#include "Text/FontCache.h"

struct hb_buffer_t;

namespace rainbow::graphics
{
    class TextureManager;
}

namespace rainbow
{
    struct GlyphPosition
    {
        uint32_t glyph_index;
        Vec2f position;
    };

    // TODO: This should include color.
    struct TextAttributes
    {
        const std::string& font_face;
        int font_size;
        TextAlignment text_alignment;
    };

    class Typesetter : public Global<Typesetter>
    {
    public:
        Typesetter(graphics::TextureManager&);
        ~Typesetter();

        auto font_cache() -> FontCache& { return font_cache_; }

        auto draw_text(std::string_view text,
                       Vec2f position,
                       const TextAttributes& attributes,
                       Vec2f* size = nullptr) -> std::vector<SpriteVertex>;

        auto layout_text(std::string_view text,
                         const TextAttributes& attributes,
                         Vec2f* size = nullptr) -> std::vector<GlyphPosition>;

    private:
        FontCache font_cache_;
        hb_buffer_t* buffer_;
    };
}  // namespace rainbow

#endif
