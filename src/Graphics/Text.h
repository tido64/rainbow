/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef TEXT_H_
#define TEXT_H_

#include <cstring>

#include "Graphics/Drawable.h"
#include "Graphics/FontAtlas.h"

class Text : public Drawable
{
public:
	Text();
	~Text();

	/// Draw text at previously set position.
	void draw();

	/// Set text color.
	void set_color(const Colorb &);

	/// Set text font.
	void set_font(const FontAtlas *);

	/// Set position of text.
	void set_position(const Vec2f &);

	/// Set text to display.
	void set_text(const char *);

	/// Does absolutely nothing (for now).
	void update();

private:
	unsigned int length;    ///< Length of the string (x 4)
	unsigned int size;      ///< Size of the vertex array
	const FontAtlas *font;  ///< The font (obviously)
	SpriteVertex *vx;       ///< Vertex array containing the text
	Vec2f position;         ///< Position of the text (top left)
	Colorb color;           ///< Color of the text
};

inline Text::Text() :
	length(0), font(nullptr), vx(nullptr) { }

inline Text::~Text()
{
	delete[] this->vx;
}

inline void Text::set_color(const Colorb &c)
{
	this->color = c;
}

inline void Text::set_font(const FontAtlas *f)
{
	this->font = f;
}

inline void Text::set_position(const Vec2f &p)
{
	this->position = p;
}

inline void Text::update() { }

#endif
