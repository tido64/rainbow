#ifndef LABEL_H_
#define LABEL_H_

#include <cstring>

#include "Graphics/Drawable.h"
#include "Graphics/FontAtlas.h"

/// Label for displaying text.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Label : public Drawable
{
public:
	inline Label();
	virtual ~Label();

	/// Set text color.
	inline void set_color(const Colorb &);

	/// Set text font.
	inline void set_font(FontAtlas *);

	/// Set position of text.
	inline void set_position(const int x, const int y);

	/// Set text to display.
	void set_text(const char *);

	/// Draw text at previously set position.
	virtual void draw();

	/// Populate the vertex array.
	virtual void update();

protected:
	static const unsigned char stale_color    = 0x01;
	static const unsigned char stale_position = 0x02;
	static const unsigned char stale_vbo      = 0x04;

private:
	unsigned char stale;       ///< Flags indicating need for update.
	size_t size;               ///< Size of the char array.
	size_t vertices;           ///< Number of vertices to draw.
	char *text;                ///< Content of this label.
	SpriteVertex *vx;          ///< Vertex array containing the text.
	SmartPtr<FontAtlas> font;  ///< The font used in this label.
	Vec2f position;            ///< Position of the text (top left).
	Colorb color;              ///< Color of the text.
};

Label::Label() :
	stale(0), size(0), vertices(0), text(nullptr), vx(nullptr) { }

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
	this->stale |= stale_position;
}

#endif
