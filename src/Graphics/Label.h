#ifndef LABEL_H_
#define LABEL_H_

#include "Graphics/Drawable.h"
#include "Graphics/FontAtlas.h"

/// Label for displaying text.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Label : public Drawable, public NonCopyable<Label>
{
public:
	enum Alignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	inline Label();
	virtual ~Label();

	/// Set text alignment.
	inline void set_alignment(const Alignment);

	/// Set text color.
	inline void set_color(const Colorb &);

	/// Set text font.
	inline void set_font(FontAtlas *);

	/// Set position of text.
	inline void set_position(const int x, const int y);

	/// Set text to display.
	void set_text(const char *);

	/// Draw text at previously set position.
	virtual void draw() override;

	/// Populate the vertex array.
	virtual void update() override;

protected:
	static const unsigned char stale_buffer = 1u << 0;
	static const unsigned char stale_color  = 1u << 1;
	static const unsigned char stale_vbo    = 1u << 2;

private:
	unsigned char stale;       ///< Flags indicating need for update.
	Colorb color;              ///< Color of the text.
	Alignment alignment;       ///< Text alignment.
	size_t size;               ///< Size of the char array.
	size_t vertices;           ///< Number of vertices to draw.
	char *text;                ///< Content of this label.
	SpriteVertex *vx;          ///< Vertex array containing the text.
	SmartPtr<FontAtlas> font;  ///< The font used in this label.
	Vec2f position;            ///< Position of the text (top left).
};

Label::Label() :
	stale(0), alignment(Label::LEFT), size(0), vertices(0), text(nullptr), vx(nullptr) { }

void Label::set_alignment(const Alignment a)
{
	this->alignment = a;
	this->stale |= stale_buffer;
}

void Label::set_color(const Colorb &c)
{
	this->color = c;
	this->stale |= stale_color;
}

void Label::set_font(FontAtlas *f)
{
	this->font = f;
}

void Label::set_position(const int x, const int y)
{
	this->position.x = x;
	this->position.y = y;
	this->stale |= stale_buffer;
}

#endif
