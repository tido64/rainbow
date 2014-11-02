// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
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
	inline const Colorb& color() const;

	/// Returns the vertex count.
	inline size_t count() const;

	/// Returns the assigned font.
	inline const FontAtlas& font() const;

	/// Returns label position.
	inline const Vec2f& position() const;

	/// Returns the string.
	inline const char* text() const;

	/// Returns the vertex array object.
	inline const VertexArray& vertex_array() const;

	/// Returns label width.
	inline unsigned int width() const;

	/// Sets text alignment.
	void set_alignment(const TextAlignment);

	/// Sets text color.
	void set_color(const Colorb &);

	/// Sets text font.
	void set_font(SharedPtr<FontAtlas>);

	/// Sets position of text.
	void set_position(const Vec2f &);

	/// Sets label scale. Value is clamped between 0.01 and 1.0.
	void set_scale(const float f);

	/// Sets label as needing update.
	inline void set_needs_update(const unsigned int what);

	/// Sets text to display.
	void set_text(const char *);

	/// Binds all used textures.
	void bind_textures() const;

	/// Moves label by (x,y).
	void move(const Vec2f &);

	/// Populates the vertex array.
	void update();

private:
	std::unique_ptr<SpriteVertex[]> vertices_;  ///< Client vertex buffer.
	std::unique_ptr<char[]> text_;  ///< Content of this label.
	size_t size_;                   ///< Size of the char array.
	Vec2f position_;                ///< Position of the text (bottom left).
	Colorb color_;                  ///< Color of the text.
	float scale_;                   ///< Label scale factor.
	TextAlignment alignment_;       ///< Text alignment.
	unsigned int count_;            ///< Number of characters * 4 (i.e. vertices).
	unsigned int stale_;            ///< Flags indicating need for update.
	unsigned int width_;            ///< Label width.
	Buffer buffer_;                 ///< Vertex buffer.
	VertexArray array_;             ///< Vertex array object.
	SharedPtr<FontAtlas> font_;     ///< The font used in this label.

	/// Aligns individual characters.
	/// \param length  Negative length of characters from \p start to \p end.
	/// \param start   First character to align.
	/// \param end     End character.
	void align(float length, const size_t start, const size_t end);
};

const Colorb& Label::color() const
{
	return color_;
}

size_t Label::count() const
{
	return count_ + (count_ >> 1);
}

const FontAtlas& Label::font() const
{
	return *font_.get();
}

const Vec2f& Label::position() const
{
	return position_;
}

const char* Label::text() const
{
	return text_.get();
}

const VertexArray& Label::vertex_array() const
{
	return array_;
}

unsigned int Label::width() const
{
	return width_;
}

void Label::set_needs_update(const unsigned int what)
{
	stale_ |= what;
}

#endif
