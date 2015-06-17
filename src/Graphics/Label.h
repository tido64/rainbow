// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_LABEL_H_
#define GRAPHICS_LABEL_H_

#include <memory>

#include "Graphics/Buffer.h"
#include "Graphics/FontAtlas.h"
#include "Graphics/VertexArray.h"

/// Label for displaying text.
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

	Label();

	/// Returns label text color.
	Colorb color() const { return color_; }

	/// Returns the vertex count.
	size_t count() const { return count_ + (count_ >> 1); }

	/// Returns the assigned font.
	const FontAtlas& font() const { return *font_.get(); }

	/// Returns label position.
	const Vec2f& position() const { return position_; }

	/// Returns the string.
	const char* text() const { return text_.get(); }

	/// Returns the vertex array object.
	const VertexArray& vertex_array() const { return array_; }

	/// Returns label width.
	unsigned int width() const { return width_; }

	/// Sets text alignment.
	void set_alignment(const TextAlignment);

	/// Sets text color.
	void set_color(const Colorb c);

	/// Sets text font.
	void set_font(SharedPtr<FontAtlas>);

	/// Sets label as needing update.
	void set_needs_update(const unsigned int what) { stale_ |= what; }

	/// Sets position of text.
	void set_position(const Vec2f &);

	/// Sets angle of rotation (in radian). Pivot depends on text alignment.
	void set_rotation(const float r);

	/// Sets label scale. Value is clamped between 0.01 and 1.0.
	void set_scale(const float f);

	/// Sets text to display.
	void set_text(const char *);

	/// Binds all used textures.
	void bind_textures() const { font_->bind(); }

	/// Moves label by (x,y).
	void move(const Vec2f &);

	/// Populates the vertex array.
	void update();

protected:
	unsigned int state() const { return stale_; }
	SpriteVertex* vertex_buffer() const { return vertices_.get(); }

	void clear_state() { stale_ = 0; }
	void update_internal();
	void upload() const;

private:
	using String = std::unique_ptr<char[]>;
	using VertexBuffer = std::unique_ptr<SpriteVertex[]>;

	VertexBuffer vertices_;      ///< Client vertex buffer.
	String text_;                ///< Content of this label.
	size_t size_;                ///< Size of the char array.
	Vec2f position_;             ///< Position of the text (bottom left).
	Colorb color_;               ///< Color of the text.
	float scale_;                ///< Label scale factor.
	TextAlignment alignment_;    ///< Text alignment.
	float angle_;                ///< Angle of rotation.
	unsigned int count_;         ///< Number of characters * 4 (i.e. vertices).
	unsigned int stale_;         ///< Flags indicating need for update.
	unsigned int width_;         ///< Label width.
	Buffer buffer_;              ///< Vertex buffer.
	VertexArray array_;          ///< Vertex array object.
	SharedPtr<FontAtlas> font_;  ///< The font used in this label.

	/// Saves line width and aligns the line if needed.
	/// \param start            First character of line.
	/// \param end              End character.
	/// \param width            Width of line.
	/// \param R                Rotation vector.
	/// \param needs_alignment  Whether alignment is needed.
	void save(const unsigned int start,
	          const unsigned int end,
	          const float width,
	          const Vec2f &R,
	          const bool needs_alignment);
};

#endif
