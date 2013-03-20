#ifndef LABEL_H_
#define LABEL_H_

#include "Graphics/Drawable.h"
#include "Graphics/FontAtlas.h"

/// Label for displaying text.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Label : public Drawable, private NonCopyable<Label>
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

	/// Return label text color.
	inline const Colorb& get_color() const;

	/// Return label width.
	inline unsigned int get_width() const;

	/// Set text alignment.
	inline void set_alignment(const Alignment);

	/// Set text color.
	inline void set_color(const Colorb &);

	/// Set text font.
	inline void set_font(FontAtlas *);

	/// Set position of text.
	void set_position(const Vec2f &);

	/// Set label scale. Value is clamped between 0.01 and 1.0.
	void set_scale(const float f);

	/// Set text to display.
	void set_text(const char *);

	/// Move label by (x,y).
	inline void move(const Vec2f &);

	/// Draw text at previously set position.
	virtual void draw() override;

	/// Populate the vertex array.
	virtual void update() override;

protected:
	static const unsigned char stale_buffer = 1u << 0;
	static const unsigned char stale_color  = 1u << 1;
	static const unsigned char stale_vbo    = 1u << 2;

private:
	unsigned char stale;        ///< Flags indicating need for update.
	Colorb color;               ///< Color of the text.
	float scale;                ///< Label scale factor.
	Alignment alignment;        ///< Text alignment.
	unsigned int width;         ///< Label width.
	size_t size;                ///< Size of the char array.
	size_t vertices;            ///< Number of vertices to draw.
	char *text;                 ///< Content of this label.
	SpriteVertex *vx;           ///< Vertex array containing the text.
	SharedPtr<FontAtlas> font;  ///< The font used in this label.
	Vec2f position;             ///< Position of the text (top left).

	/// Align individual characters.
	/// \param length  Negative length of characters from \p start to \p end.
	/// \param start   First character to align.
	/// \param end     End character.
	void align(float length, size_t start, size_t end);
};

Label::Label() :
	stale(0), scale(1.0f), alignment(Label::LEFT), width(0), size(0),
	vertices(0), text(nullptr), vx(nullptr) { }

const Colorb& Label::get_color() const
{
	return this->color;
}

unsigned int Label::get_width() const
{
	return this->width;
}

void Label::set_alignment(const Label::Alignment a)
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
	this->stale |= stale_buffer;
}

void Label::move(const Vec2f &delta)
{
	this->position += delta;
	this->stale |= stale_buffer;
}

#endif
