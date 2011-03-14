/// Stores screen size.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SCREEN_H_
#define SCREEN_H_

#include "Platform.h"

#ifdef RAINBOW_IOS
#	include <cocos2d/cocos2d.h>
#else
#	include <cassert>
#endif

class Screen
{
public:
	static inline Screen& Instance()
	{
		static Screen main;
		return main;
	}

	void init(const float width = 0.0f, const float height = 0.0f)
	{
	#if defined(RAINBOW_IOS)

	#else

		assert(width > 0.0f && height > 0.0f);
		this->w = width;
		this->h = height;

	#endif
	}

	inline float width() const
	{
	#if defined(RAINBOW_IOS)

		return [[CCDirector sharedDirector] winSizeInPixels].width;

	#else

		return this->h;

	#endif
	}

	inline float height() const
	{
	#if defined(RAINBOW_IOS)

		return [[CCDirector sharedDirector] winSizeInPixels].height;

	#else

		return this->h;

	#endif
	}

private:
	float w, h;

	Screen() : w(0.0f), h(0.0f) { }
	Screen(const Screen &);
	Screen& operator=(const Screen &);
};

#endif
