/// Stores screen size.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SCREEN_H_
#define SCREEN_H_

#include <Rainbow/Hardware/Platform.h>

#if defined(ONWIRE_ANDROID)
#elif defined(ONWIRE_IOS)
#	include <cocos2d/cocos2d.h>
#endif

class Screen
{
public:
	static inline Screen* Instance()
	{
		static Screen main;
		return &main;
	}

	void init(const float width = 0, const float height = 0)
	{
	#if defined(ONWIRE_IOS)

		this->w = [CCDirector sharedDirector].winSize.width;
		this->h = [CCDirector sharedDirector].winSize.height;

	#else

		this->w = width;
		this->h = height;

	#endif
	}

	inline float width() { return this->w; }
	inline float height() { return this->h; }

private:
	float w, h;

	Screen() : w(0.0f), h(0.0f) { }
	Screen(const Screen &);
	Screen& operator=(const Screen &);
};

#endif
