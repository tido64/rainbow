#ifndef GRAPHICS_LABEL_H_
#define GRAPHICS_LABEL_H_

#include <memory>

#include "Graphics/FontAtlas.h"
#include "Graphics/VertexArray.h"

class Label;
namespace Renderer { void draw(const Label &); }

/// Label for displaying text.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Label : private NonCopyable<Label>
{
	friend void Renderer::draw(const Label &);

public:
	enum Alignment
	{
		kLeftTextAlignment,
		kRightTextAlignment,
		kCenterTextAlignment
	};

	inline Label();

	/// Returns label text color.
	inline const Colorb& get_color() const;

	/// Returns label width.
	inline unsigned int get_width() const;

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
	Colorb color;                        ///< Color of the text.
	float scale;                         ///< Label scale factor.
	Alignment alignment;                 ///< Text alignment.
	unsigned int stale;                  ///< Flags indicating need for update.
	unsigned int width;                  ///< Label width.
	size_t size;                         ///< Size of the char array.
	std::unique_ptr<char[]> text;        ///< Content of this label.
	std::unique_ptr<SpriteVertex[]> vx;  ///< Vertex array containing the text.
	Vec2f position;                      ///< Position of the text (top left).
	SharedPtr<FontAtlas> font;           ///< The font used in this label.
	Renderer::VertexArray array;         ///< Vertex array object.

	/// Aligns individual characters.
	/// \param length  Negative length of characters from \p start to \p end.
	/// \param start   First character to align.
	/// \param end     End character.
	void align(float length, size_t start, size_t end);
};

Label::Label() :
	scale(1.0f), alignment(kLeftTextAlignment), stale(0), width(0), size(0) { }

const Colorb& Label::get_color() const
{
	return this->color;
}

unsigned int Label::get_width() const
{
	return this->width;
}

#endif
