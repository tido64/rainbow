/*
 *  Label.h
 *  OnWire
 *
 *	Platform independent label super class.
 *
 *  Created by Tommy Nguyen on 7/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef LABEL_H_
#define LABEL_H_

#include "Platform.h"

#if defined(ONWIRE_ANDROID)
#	// Insert definitions
#elif defined(ONWIRE_IOS)
#	define LabelObject CCBitmapFontAtlas
#endif

class Label
{
public:
	Label(const char *str, const char *font = 0) : valign(0.5f)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		NSString *s = ctons_str(str);
		NSString *f = ctons_str(font);
		this->label = [CCBitmapFontAtlas bitmapFontAtlasWithString:s fntFile:f];

	#endif
	}

	~Label() { delete this->label; }

	void align_center()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(0.5f, this->valign)];

	#endif
	}

	void align_left()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(0, this->valign)];

	#endif
	}
	
	void align_right()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setAnchorPoint: ccp(1, this->valign)];

	#endif
	}

	const unsigned int get_height()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		return this->label.contentSize.height;

	#endif
	}
	
	LabelObject *get_label() { return this->label; }

	const unsigned int get_width()
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		return this->label.contentSize.width;

	#endif
	}
	
	void set_position(const unsigned int x, const unsigned int y)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		this->label.position = ccp(x, y);

	#endif
	}

	void scale(const float f)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		[label setScale:f];

	#endif
	}
	
	
	void set_text(const char *str)
	{
	#if defined(ONWIRE_ANDROID)

	#elif defined(ONWIRE_IOS)

		NSString *s = ctons_str(str);
		[label setString:s];

	#endif
	}

	void set_valign(const float v) { this->valign = v; }
	
private:
	float valign;
	LabelObject *label;
};

#endif
