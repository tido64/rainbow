/// Platform independent label class.

/// \see http://www.cocos2d-iphone.org/forum/topic/8267
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef LABEL_H_
#define LABEL_H_

#include <Rainbow/Hardware/Platform.h>

#ifdef ONWIRE_IOS
#	include <cocos2d/cocos2d.h>
#	define ctons_str(s) [NSString stringWithUTF8String:(s)];

typedef CCLabelBMFont RealLabel;

#else
#	include <Rainbow/FontAtlas.h>
#endif

class Label
{

#ifdef ONWIRE_IOS
public:
	/// Creates a label with an initial string and font.
	/// \param str	Initial string
	/// \param font	Font to be used with this label
	Label(const char *str, const char *font = 0) : align(0.0f), valign(0.5f)
	{
		NSString *s = ctons_str(str);
		NSString *f = ctons_str(font);
		this->label = [RealLabel labelWithString:s fntFile:f];
	}

	~Label() { delete this->label; }

	/// Center-justifies the label.
	inline void align_center()
	{
		this->align = 0.5f;
		[label setAnchorPoint: ccp(this->align, this->valign)];
	}

	/// Left-justifies the label.
	inline void align_left()
	{
		this->align = 0.0f;
		[label setAnchorPoint: ccp(this->align, this->valign)];
	}

	/// Right-justifies the label.
	inline void align_right()
	{
		this->align = 1.0f;
		[label setAnchorPoint: ccp(this->align, this->valign)];
	}

	/// Returns the height of the label.
	inline const unsigned int get_height()
	{
		return this->label.contentSize.height;
	}

	/// Returns the label object. Used by the director.
	/// \return Actual label object
	inline RealLabel *get_label() { return this->label; }

	/// Returns the width of the label.
	inline const unsigned int get_width()
	{
		return this->label.contentSize.width;
	}

	/// Sets the position of the label.
	/// \param x The x-coordinate
	/// \param y The y-coordinate
	inline void set_position(const unsigned int x, const unsigned int y)
	{
		this->label.position = ccp(x, y);
	}

	/// Scales the label to given factor.
	/// \param f Factor to scale by
	inline void scale(const float &f)
	{
		[label setScale:f];
	}

	/// Sets the text in the label.
	/// \param str The text
	void set_text(const char *str)
	{
		NSString *s = ctons_str(str);
		[label setString:s];
	}

	/// Sets the vertial alignment.
	/// \param v Vertical alignment value between 0 and 1
	inline void set_valign(const float &v)
	{
		this->valign = v;
		[label setAnchorPoint: ccp(this->align, this->valign)];
	}

private:
	float align;       ///< Horisontal alignment
	float valign;      ///< Vertical alignment
	RealLabel *label;  ///< Actual label object

#else

public:
	Label(const char *str = 0, const FontAtlas *f = 0) : string(str), font(f) { }

	void draw()
	{
		if (this->string == 0) return;

		this->font->print(this->string, this->position.x, this->position.y);
	}

	inline void set_font(const FontAtlas *f) { this->font = f; }

	void set_position(const float x, const float y)
	{
		this->position.x = x;
		this->position.y = y;
	}

	void set_text(const char *str)
	{
		this->string = str;
	}

private:
	const char *string;
	const FontAtlas *font;
	Vec2f position;

#endif
};

#endif
