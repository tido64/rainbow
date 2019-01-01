// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LABEL_H_
#define GRAPHICS_LABEL_H_

#include <string>
#include <vector>

#include <Rainbow/TextAlignment.h>

#include "Common/Color.h"
#include "Common/TypeCast.h"
#include "Graphics/LifetimeTracked.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/Vulkan.h"
#include "Math/Vec2.h"

namespace rainbow
{
    namespace graphics
    {
        class Driver;
    }

    /// <summary>Label for displaying text.</summary>
    class Label : public LifetimeTracked<Label>, NonCopyable<Label>
    {
    public:
        static constexpr uint32_t kStaleBuffer = 1u << 0;
        static constexpr uint32_t kStaleBufferSize = 1u << 1;
        static constexpr uint32_t kStaleColor = 1u << 2;
        static constexpr uint32_t kStaleMask = 0xffffu;

        Label(const graphics::Driver&);
        virtual ~Label() = default;

        /// <summary>Returns label text alignment.</summary>
        auto alignment() const { return alignment_; }

        /// <summary>Returns label angle of rotation.</summary>
        auto angle() const { return angle_; }

        /// <summary>Returns label text color.</summary>
        auto color() const { return color_; }

        /// <summary>Returns the assigned font.</summary>
        auto font() const -> const std::string& { return font_face_; }

        /// <summary>Returns font size.</summary>
        auto font_size() const { return font_size_; }

        /// <summary>Returns label height.</summary>
        auto height() const { return size_.y; }

        /// <summary>Returns the number of characters.</summary>
        auto length() const
        {
            return narrow_cast<uint32_t>(vertices_.size() / 4);
        }

        /// <summary>Returns label position.</summary>
        auto position() const { return position_; }

        /// <summary>Returns label scale.</summary>
        auto scale() const { return scale_; }

        /// <summary>Returns the string.</summary>
        auto text() const { return text_.c_str(); }

        /// <summary>Returns the vertex buffer.</summary>
        auto vertex_buffer() const -> const vk::DynamicVertexBuffer&
        {
            return buffer_;
        }

        /// <summary>Returns the vertex count.</summary>
        auto vertex_count() const
        {
            const auto count = vertices_.size();
            return narrow_cast<uint32_t>(count + (count >> 1));
        }

        /// <summary>Returns label width.</summary>
        auto width() const { return size_.x; }

        /// <summary>Sets text alignment.</summary>
        void set_alignment(TextAlignment);

        /// <summary>Sets text color.</summary>
        void set_color(Color c);

        /// <summary>Sets text font.</summary>
        void set_font(std::string font_face);

        /// <summary>Sets font size.</summary>
        void set_font_size(int font_size);

        /// <summary>Sets label as needing update.</summary>
        void set_needs_update(uint32_t what) { stale_ |= what; }

        /// <summary>Sets position of text.</summary>
        void set_position(Vec2f);

        /// <summary>
        ///   Sets angle of rotation (in radian). Pivot depends on text
        ///   alignment.
        /// </summary>
        void set_rotation(float r);

        /// <summary>
        ///   Sets label scale. Value is clamped between 0.01 and 1.0.
        /// </summary>
        void set_scale(float f);

        /// <summary>Sets text to display.</summary>
        void set_text(std::string);

        /// <summary>Moves label by (x,y).</summary>
        void move(Vec2f);

        /// <summary>Populates the vertex array.</summary>
        virtual void update();

    protected:
        auto state() const { return stale_; }
        auto vertices() const { return vertices_.data(); }

        void clear_state() { stale_ = 0; }
        void update_internal();
        void upload();

    private:
        /// <summary>Flags indicating need for update.</summary>
        uint32_t stale_;

        /// <summary>Vertex buffer.</summary>
        vk::DynamicVertexBuffer buffer_;

        /// <summary>Client vertex buffer.</summary>
        std::vector<SpriteVertex> vertices_;

        /// <summary>Content of this label.</summary>
        std::string text_;

        /// <summary>Font used to draw the text.</summary>
        std::string font_face_;

        /// <summary>Font size used to draw the text.</summary>
        int font_size_;

        /// <summary>Text alignment.</summary>
        TextAlignment alignment_;

        /// <summary>Position of the text (bottom left).</summary>
        Vec2f position_;

        /// <summary>Text colour.</summary>
        Color color_;

        /// <summary>Label scale factor.</summary>
        float scale_;

        /// <summary>Angle of rotation.</summary>
        float angle_;

        /// <summary>Label size.</summary>
        Vec2f size_;
    };

    namespace vk
    {
        void draw(const CommandBuffer&, const Label&, const IndexBuffer&);
    }
}  // namespace rainbow

#endif
