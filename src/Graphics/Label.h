#ifndef LABEL_H_
#define LABEL_H_

#include "Graphics/FontAtlas.h"
#include "Graphics/VertexArray.h"

/// Label for displaying text.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Label : private NonCopyable<Label>
{
public:
	enum Alignment
	{
		kLeftTextAlignment,
		kRightTextAlignment,
		kCenterTextAlignment
	};

	inline Label();
	virtual ~Label();

	/// Return label text color.
	inline const Colorb& get_color() const;

	/// Return label width.
	inline unsigned int get_width() const;

	/// Set text alignment.
	void set_alignment(const Alignment);

	/// Set text color.
	void set_color(const Colorb &);

	/// Set text font.
	void set_font(FontAtlas *);

	/// Set position of text.
	void set_position(const Vec2f &);

	/// Set label scale. Value is clamped between 0.01 and 1.0.
	void set_scale(const float f);

	/// Set text to display.
	void set_text(const char *);

	/// Move label by (x,y).
	void move(const Vec2f &);

	/// Draw text at previously set position.
	void draw();

	/// Populate the vertex array.
	void update();

private:
	Colorb color;                 ///< Color of the text.
	float scale;                  ///< Label scale factor.
	Alignment alignment;          ///< Text alignment.
	unsigned int stale;           ///< Flags indicating need for update.
	unsigned int width;           ///< Label width.
	size_t size;                  ///< Size of the char array.
	char *text;                   ///< Content of this label.
	SpriteVertex *vx;             ///< Vertex array containing the text.
	Vec2f position;               ///< Position of the text (top left).
	SharedPtr<FontAtlas> font;    ///< The font used in this label.
	Renderer::VertexArray array;  ///< Vertex array object.

	/// Align individual characters.
	/// \param length  Negative length of characters from \p start to \p end.
	/// \param start   First character to align.
	/// \param end     End character.
	void align(float length, size_t start, size_t end);
};

Label::Label() :
	scale(1.0f), alignment(kLeftTextAlignment), stale(0), width(0), size(0),
	text(nullptr), vx(nullptr) { }

const Colorb& Label::get_color() const
{
	return this->color;
}

unsigned int Label::get_width() const
{
	return this->width;
}

#endif
