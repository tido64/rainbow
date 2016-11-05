// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LABEL_H_
#define GRAPHICS_LABEL_H_

#include <memory>

#include "Graphics/Buffer.h"
#include "Graphics/FontAtlas.h"
#include "Graphics/VertexArray.h"

/// <summary>Label for displaying text.</summary>
class Label : private NonCopyable<Label>
{
public:
    enum class TextAlignment
    {
        Left,
        Right,
        Center
    };

    static const unsigned int kStaleBuffer      = 1u << 0;
    static const unsigned int kStaleBufferSize  = 1u << 1;
    static const unsigned int kStaleColor       = 1u << 2;
    static const unsigned int kStaleMask        = 0xffffu;

    Label();
    virtual ~Label() = default;

    /// <summary>Returns label text alignment.</summary>
    auto alignment() const { return alignment_; }

    /// <summary>Returns label angle of rotation.</summary>
    auto angle() const { return angle_; }

    /// <summary>Returns label text color.</summary>
    auto color() const { return color_; }

    /// <summary>Returns the assigned font.</summary>
    auto font() const -> const FontAtlas& { return *font_.get(); }

    /// <summary>Returns the number of characters.</summary>
    auto length() const { return count_ / 4; }

    /// <summary>Returns label position.</summary>
    auto position() const -> const Vec2f& { return position_; }

    /// <summary>Returns label scale.</summary>
    auto scale() const { return scale_; }

    /// <summary>Returns the string.</summary>
    auto text() const { return text_.get(); }

    /// <summary>Returns the vertex array object.</summary>
    auto vertex_array() const -> const rainbow::graphics::VertexArray&
    {
        return array_;
    }

    /// <summary>Returns the vertex count.</summary>
    auto vertex_count() const
    {
        return std::min(count_ + (count_ >> 1), cutoff_);
    }

    /// <summary>Returns label width.</summary>
    auto width() const { return width_; }

    /// <summary>Sets text alignment.</summary>
    void set_alignment(TextAlignment);

    /// <summary>Sets text color.</summary>
    void set_color(Colorb c);

    /// <summary>Sets text font.</summary>
    void set_font(SharedPtr<FontAtlas>);

    /// <summary>Sets label as needing update.</summary>
    void set_needs_update(unsigned int what) { stale_ |= what; }

    /// <summary>Sets position of text.</summary>
    void set_position(const Vec2f&);

    /// <summary>
    ///   Sets angle of rotation (in radian). Pivot depends on text alignment.
    /// </summary>
    void set_rotation(float r);

    /// <summary>
    ///   Sets label scale. Value is clamped between 0.01 and 1.0.
    /// </summary>
    void set_scale(float f);

    /// <summary>Sets text to display.</summary>
    void set_text(const char*);

    /// <summary>Binds all used textures.</summary>
    void bind_textures() const { font_->bind(); }

    /// <summary>Moves label by (x,y).</summary>
    void move(const Vec2f&);

    /// <summary>Populates the vertex array.</summary>
    virtual void update();

protected:
    auto cutoff() const -> int { return cutoff_ / 6; }
    void set_cutoff(int cutoff) { cutoff_ = cutoff * 6; }

    auto state() const { return stale_; }
    auto vertex_buffer() const { return vertices_.get(); }

    void clear_state() { stale_ = 0; }
    void update_internal();
    void upload() const;

private:
    using String = std::unique_ptr<char[]>;
    using VertexBuffer = std::unique_ptr<SpriteVertex[]>;

    VertexBuffer vertices_;      ///< Client vertex buffer.
    String text_;                ///< Content of this label.
    Vec2f position_;             ///< Position of the text (bottom left).
    Colorb color_;               ///< Color of the text.
    float scale_;                ///< Label scale factor.
    TextAlignment alignment_;    ///< Text alignment.
    float angle_;                ///< Angle of rotation.
    unsigned int count_;         ///< Number of characters * 4 (i.e. vertices).
    unsigned int stale_;         ///< Flags indicating need for update.
    unsigned int width_;         ///< Label width.
    unsigned int cutoff_;        ///< Number of characters to render.
    size_t size_;                ///< Size of the char array.
    rainbow::graphics::Buffer buffer_;      ///< Vertex buffer.
    rainbow::graphics::VertexArray array_;  ///< Vertex array object.
    SharedPtr<FontAtlas> font_;  ///< The font used in this label.

    /// <summary>Saves line width and aligns the line if needed.</summary>
    /// <param name="start">First character of line.</param>
    /// <param name="end">End character.</param>
    /// <param name="width">Width of line.</param>
    /// <param name="R">Rotation vector.</param>
    /// <param name="needs_alignment">Whether alignment is needed.</param>
    void save(unsigned int start,
              unsigned int end,
              float width,
              const Vec2f& R,
              bool needs_alignment);
};

#endif
