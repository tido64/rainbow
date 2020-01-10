// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LABEL_H_
#define GRAPHICS_LABEL_H_

#include <memory>
#include <string>
#include <vector>

#include <Rainbow/TextAlignment.h>

#include "Common/Color.h"
#include "Common/String.h"
#include "Common/TypeCast.h"
#include "Graphics/Buffer.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/VertexArray.h"
#include "Math/Vec2.h"

namespace rainbow
{
    class GameBase;

    /// <summary>Label for displaying text.</summary>
    class Label : private NonCopyable<Label>
    {
    public:
        // clang-format off
        static constexpr uint32_t kStaleBuffer      = 1U << 0;
        static constexpr uint32_t kStaleBufferSize  = 1U << 1;
        static constexpr uint32_t kStaleColor       = 1U << 2;
        static constexpr uint32_t kStaleMask        = 0xffffU;
        // clang-format on

        Label();
        ~Label();

        /// <summary>Returns label text alignment.</summary>
        [[nodiscard]] auto alignment() const { return alignment_; }

        /// <summary>Returns label angle of rotation.</summary>
        [[nodiscard]] auto angle() const { return angle_; }

        /// <summary>Returns label text color.</summary>
        [[nodiscard]] auto color() const { return color_; }

        /// <summary>Returns the assigned font.</summary>
        [[nodiscard]] auto font() const -> std::string_view
        {
            return font_face_;
        }

        /// <summary>Returns font size.</summary>
        [[nodiscard]] auto font_size() const { return font_size_; }

        /// <summary>Returns label height.</summary>
        [[nodiscard]] auto height() const { return size_.y; }

        /// <summary>Returns the number of characters.</summary>
        [[nodiscard]] auto length() const
        {
            return narrow_cast<uint32_t>(vertices_.size() / 4);
        }

        /// <summary>Returns label position.</summary>
        [[nodiscard]] auto position() const { return position_; }

        /// <summary>Returns label scale.</summary>
        [[nodiscard]] auto scale() const { return scale_; }

        /// <summary>Returns the string.</summary>
        [[nodiscard]] auto text() const { return text_.c_str(); }

        /// <summary>Returns the vertex array object.</summary>
        [[nodiscard]] auto vertex_array() const -> const graphics::VertexArray&
        {
            return array_;
        }

        /// <summary>Returns the vertex count.</summary>
        [[nodiscard]] auto vertex_count() const
        {
            const auto count = vertices_.size();
            return narrow_cast<int>(count + (count >> 1));
        }

        /// <summary>Returns label width.</summary>
        [[nodiscard]] auto width() const { return size_.x; }

        /// <summary>Sets text alignment.</summary>
        auto alignment(TextAlignment) -> Label&;

        /// <summary>
        ///   Sets angle of rotation (in radian). Pivot depends on text
        ///   alignment.
        /// </summary>
        auto angle(float r) -> Label&;

        /// <summary>Sets text color.</summary>
        auto color(Color c) -> Label&;

        /// <summary>Sets text font.</summary>
        auto font(czstring font_face) -> Label&;

        /// <summary>Sets font size.</summary>
        auto font_size(int font_size) -> Label&;

        /// <summary>Moves label by (x,y).</summary>
        auto move(Vec2f) -> Label&;

        /// <summary>Sets position of text.</summary>
        auto position(Vec2f) -> Label&;

        /// <summary>
        ///   Sets label scale. Value is clamped between 0.01 and 1.0.
        /// </summary>
        auto scale(float f) -> Label&;

        /// <summary>Sets text to display.</summary>
        auto text(czstring) -> Label&;

        /// <summary>Populates the vertex array.</summary>
        void update(GameBase&);

    protected:
        [[nodiscard]] auto state() const { return stale_; }
        [[nodiscard]] auto vertex_buffer() const { return vertices_.data(); }

        void clear_state() { stale_ = 0; }

        /// <summary>Sets label as needing update.</summary>
        void set_needs_update(unsigned int what) { stale_ |= what; }

        void update_internal(GameBase&);
        void upload() const;

    private:
        /// <summary>Flags indicating need for update.</summary>
        unsigned int stale_ = 0;

        /// <summary>Vertex array object.</summary>
        graphics::VertexArray array_;

        /// <summary>Client vertex buffer.</summary>
        std::vector<SpriteVertex> vertices_;

        /// <summary>Content of this label.</summary>
        std::string text_;

        /// <summary>Font used to draw the text.</summary>
        std::string font_face_;

        /// <summary>Font size used to draw the text.</summary>
        int font_size_ = 15;

        /// <summary>Text alignment.</summary>
        TextAlignment alignment_ = TextAlignment::Left;

        /// <summary>Position of the text (bottom left).</summary>
        Vec2f position_;

        /// <summary>Text colour.</summary>
        Color color_;

        /// <summary>Label scale factor.</summary>
        float scale_ = 1.0F;

        /// <summary>Angle of rotation.</summary>
        float angle_ = 0.0F;

        /// <summary>Label size.</summary>
        Vec2f size_;

        /// <summary>Vertex buffer.</summary>
        graphics::Buffer buffer_;
    };
}  // namespace rainbow

namespace rainbow::graphics
{
    struct Context;

    void draw(Context&, const Label&);
}  // namespace rainbow::graphics

#endif
