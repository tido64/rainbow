/// Platform independent label class.

/// \see http://www.cocos2d-iphone.org/forum/topic/8267
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef LABEL_H_
#define LABEL_H_

#include <Rainbow/Hardware/Platform.h>

#if defined(ONWIRE_ANDROID)

#elif defined(ONWIRE_IOS)

#include <cocos2d/cocos2d.h>

typedef CCLabelBMFont RealLabel;

#endif

class Label
{
public:

	/// Creates a label with an initial string and font.
	/// \param str	Initial string
	/// \param font	Font to be used with this label
	Label(const char *str, const char *font = 0) : align(0.0f), valign(0.5f)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		NSString *s = ctons_str(str);
		NSString *f = ctons_str(font);
		this->label = [RealLabel labelWithString:s fntFile:f];

	#endif
	}

	~Label() { delete this->label; }

	/// Center-justifies the label.
	inline void align_center()
	{
		this->align = 0.5f;

	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(this->align, this->valign)];

	#endif
	}

	/// Left-justifies the label.
	inline void align_left()
	{
		this->align = 0.0f;

	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(this->align, this->valign)];

	#endif
	}

	/// Right-justifies the label.
	inline void align_right()
	{
		this->align = 1.0f;

	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(this->align, this->valign)];

	#endif
	}

	/// Returns the height of the label.
	inline const unsigned int get_height()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		return this->label.contentSize.height;

	#endif
	}

	/// Returns the label object. Used by the director.
	/// \return Actual label object
	inline RealLabel *get_label() { return this->label; }

	/// Returns the width of the label.
	inline const unsigned int get_width()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		return this->label.contentSize.width;

	#endif
	}

	/// Sets the position of the label.
	/// \param x The x-coordinate
	/// \param y The y-coordinate
	inline void set_position(const unsigned int x, const unsigned int y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->label.position = ccp(x, y);

	#endif
	}

	/// Scales the label to given factor.
	/// \param f Factor to scale by
	inline void scale(const float &f)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setScale:f];

	#endif
	}

	/// Sets the text in the label.
	/// \param str The text
	void set_text(const char *str)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		NSString *s = ctons_str(str);
		[label setString:s];

	#endif
	}

	/// Sets the vertial alignment.
	/// \param v Vertical alignment value between 0 and 1
	inline void set_valign(const float &v)
	{
		this->valign = v;
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(this->align, this->valign)];

	#endif
	}

private:
	float align;       ///< Horisontal alignment
	float valign;      ///< Vertical alignment
	RealLabel *label;  ///< Actual label object
};

#endif
