// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Text/Typesetter.h"

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <hb.h>  // NOLINT(llvm-include-order)
#include <hb-ft.h>
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include "Common/Logging.h"
#include "Common/String.h"
#include "Common/TypeCast.h"

using rainbow::czstring;
using rainbow::GlyphPosition;
using rainbow::SpriteVertex;
using rainbow::TextAttributes;
using rainbow::Typesetter;
using rainbow::Vec2f;

namespace
{
    /// <summary>Horizontal/vertical resolution in dpi.</summary>
    constexpr uint32_t kDPI = 96;

    /// <summary>26.6 fixed-point pixel coordinates.</summary>
    constexpr int kPixelFormat = 64;

    template <typename Iterator>
    void offset_by(Iterator begin, Iterator end, float offset)
    {
        std::for_each(
            begin, end, [offset](auto&& glyph) { glyph.position.x -= offset; });
    }

    auto suggest_line_break(std::string_view text, int start) -> int
    {
        const auto length = rainbow::narrow_cast<int>(text.length());
        int i = 0;
        while (start + i < length && text[start + i] != '\n')
            ++i;
        return i;
    }

    constexpr auto to_vec2(hb_position_t x, hb_position_t y) -> Vec2f
    {
        return Vec2f{x / rainbow::narrow_cast<float>(kPixelFormat),
                     y / rainbow::narrow_cast<float>(kPixelFormat)};
    }
}  // namespace

Typesetter::Typesetter()
{
    buffer_ = hb_buffer_create();
    R_ASSERT(hb_buffer_allocation_successful(buffer_),  //
             "Failed to allocate HarfBuzz buffer");

    make_global();
}

Typesetter::~Typesetter()
{
    hb_buffer_destroy(buffer_);
}

auto Typesetter::draw_text(std::string_view text,
                           const Vec2f& position,
                           const TextAttributes& attributes,
                           Vec2f* size) -> std::vector<SpriteVertex>
{
    auto glyph_positions = layout_text(text, attributes, size);
    std::vector<SpriteVertex> vertices;
    vertices.reserve(glyph_positions.size() * 4);
    auto font_face = font_cache_.get(attributes.font_face);
    for (auto&& glyph : glyph_positions)
    {
        auto vx = font_cache_.get_glyph(
            font_face, attributes.font_size, glyph.glyph_index);
        auto p = glyph.position + position;
        vx[0].position += p;
        vx[1].position += p;
        vx[2].position += p;
        vx[3].position += p;
        vertices.insert(vertices.end(), vx.begin(), vx.end());
    }
    return vertices;
}

auto Typesetter::layout_text(std::string_view text,
                             const TextAttributes& attributes,
                             Vec2f* size) -> std::vector<GlyphPosition>
{
    std::vector<GlyphPosition> result;

    auto font_face = font_cache_.get(attributes.font_face);
    FT_Set_Char_Size(
        font_face, 0, attributes.font_size * kPixelFormat, 0, kDPI);
    const auto line_height =
        font_face->size->metrics.height / narrow_cast<float>(kPixelFormat);
    auto font = hb_ft_font_create(font_face, nullptr);
    hb_ft_font_set_load_flags(font, FT_LOAD_DEFAULT);

    float width = 0.0F;
    int line_count = 0;
    int start = 0;
    const auto length = narrow_cast<int>(text.length());
    while (start < length)
    {
        hb_buffer_reset(buffer_);

        const int line_length = suggest_line_break(text, start);
        hb_buffer_add_utf8(buffer_, text.data(), length, start, line_length);
        hb_buffer_guess_segment_properties(buffer_);
        hb_shape(font, buffer_, nullptr, 0);

        unsigned int count;
        auto infos = hb_buffer_get_glyph_infos(buffer_, &count);
        auto positions = hb_buffer_get_glyph_positions(buffer_, &count);

        Vec2f origin{0, -line_height * line_count};
        for (unsigned int i = 0; i < count; ++i)
        {
            const auto& p = positions[i];
            result.push_back(
                {infos[i].codepoint, origin + to_vec2(p.x_offset, p.y_offset)});
            origin += to_vec2(p.x_advance, p.y_advance);
        }

        switch (attributes.text_alignment)
        {
            case TextAlignment::Left:
                break;
            case TextAlignment::Right:
                offset_by(result.begin() + result.size() - count,
                          result.end(),
                          origin.x);
                break;
            case TextAlignment::Center:
                offset_by(result.begin() + result.size() - count,
                          result.end(),
                          origin.x / 2);
                break;
        }

        start += line_length + 1;  // Skip newline
        ++line_count;

        width = std::max(width, origin.x);
    }

    hb_font_destroy(font);

    if (size != nullptr)
    {
        size->x = width;
        size->y = line_height * line_count;
    }

    return result;
}
