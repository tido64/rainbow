#ifndef SCREEN_H_
#define SCREEN_H_

#include "Platform.h"

#ifdef RAINBOW_IOS
#	include <cocos2d/cocos2d.h>
#else
#	include <cassert>
#endif

/// Stores screen size.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Screen
{
public:
	static inline Screen& Instance()
	{
		static Screen main;
		return main;
	}

	void set(const float width = 0.0f, const float height = 0.0f)
	{
	#if defined(RAINBOW_IOS)

		CGSize dim = [[CCDirector sharedDirector] winSizeInPixels];
		this->w = dim.width;
		this->h = dim.height;

	#else

		assert(width > 0.0f && height > 0.0f);
		this->w = width;
		this->h = height;

	#endif
	}

	float width() const;
	float height() const;

private:
	float w, h;

	Screen() : w(0.0f), h(0.0f)
	{
	#if defined(RAINBOW_IOS)
		this->set();
	#endif
	}

	Screen(const Screen &);
	Screen& operator=(const Screen &);
};

inline float Screen::width() const
{
	return this->w;
}

inline float Screen::height() const
{
	return this->h;
}

#endif
