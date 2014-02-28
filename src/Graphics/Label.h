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
	enum Alignment
	{
		kLeftTextAlignment,
		kRightTextAlignment,
		kCenterTextAlignment
	};

	Label();

	/// Returns label text color.
	inline const Colorb& color() const;

	/// Returns the vertex count.
	inline size_t count() const;

	/// Returns the vertex array object.
	inline const VertexArray& vertex_array() const;

	/// Returns label width.
	inline unsigned int width() const;

	/// Sets text alignment.
	void set_alignment(const Alignment);

	/// Sets text color.
	void set_color(const Colorb &);

	/// Sets text font.
	void set_font(FontAtlas *);

	/// Sets position of text.
	void set_position(const Vec2f &);

	/// Sets label scale. Value is clamped between 0.01 and 1.0.
	void set_scale(const float f);

	/// Sets text to display.
	void set_text(const char *);

	/// Moves label by (x,y).
	void move(const Vec2f &);

	/// Populates the vertex array.
	void update();

private:
	Colorb color_;                  ///< Color of the text.
	float scale_;                   ///< Label scale factor.
	Alignment alignment_;           ///< Text alignment.
	unsigned int count_;            ///< Number of characters * 4 (i.e. vertices).
	unsigned int stale_;            ///< Flags indicating need for update.
	unsigned int width_;            ///< Label width.
	size_t size_;                   ///< Size of the char array.
	std::unique_ptr<char[]> text_;  ///< Content of this label.
	Vec2f position_;                ///< Position of the text (top left).
	SharedPtr<FontAtlas> font_;     ///< The font used in this label.
	Buffer<SpriteVertex> buffer_;   ///< Vertex buffer.
	VertexArray array_;             ///< Vertex array object.

	/// Aligns individual characters.
	/// \param length  Negative length of characters from \p start to \p end.
	/// \param start   First character to align.
	/// \param end     End character.
	void align(float length, size_t start, size_t end);
};

const Colorb& Label::color() const
{
	return this->color_;
}

size_t Label::count() const
{
	return this->count_ + (this->count_ >> 1);
}

const VertexArray& Label::vertex_array() const
{
	return this->array_;
}

unsigned int Label::width() const
{
	return this->width_;
}

#endif
