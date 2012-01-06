#ifndef LABEL_H_
#define LABEL_H_

#include <cstring>

#include "Graphics/Drawable.h"
#include "Graphics/FontAtlas.h"

/// Label for displaying text.
///
/// \note Current algorithm is not optimized in any way. Text is rendered
///       character by character (ie. a glDraw per character) without any form
///       of caching.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Label : public Drawable
{
public:
	Label();
	Label(const char *);
	~Label();

	/// Draw text at previously set position.
	virtual void draw();

	/// Render string to a bitmap.
	void finalize();

	/// Set text color.
	void set_color(const Colorb &);

	/// Set text font.
	void set_font(FontAtlas *);

	/// Set position of text.
	void set_position(const int x, const int y);

	/// Set text to display.
	void set_text(const char *);

	/// Does absolutely nothing (for now).
	virtual void update();

private:
	unsigned int length;       ///< Length of the string.
	unsigned int size;         ///< Size of the vertex array.
	char *text;                ///< Content of this label.
	SpriteVertex *vx;          ///< Vertex array containing the text.
	SmartPtr<FontAtlas> font;  ///< The font used in this label.
	Vec2f position;            ///< Position of the text (top left).
	Colorb color;              ///< Color of the text.
};

inline Label::Label() :
	length(0), size(0), text(nullptr), vx(nullptr) { }

inline void Label::set_color(const Colorb &c)
{
	this->color = c;
}

inline void Label::set_font(FontAtlas *f)
{
	this->font = f;
}

inline void Label::set_position(const int x, const int y)
{
	this->position.x = x;
	this->position.y = y;
}

inline void Label::update() { }

#endif
