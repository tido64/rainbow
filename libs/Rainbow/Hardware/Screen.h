/// Retrieves screen size regardless of platform.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef SCREEN_H_
#define SCREEN_H_

#include <Rainbow/Hardware/Platform.h>

#if defined(ONWIRE_ANDROID)
#elif defined(ONWIRE_IOS)
#	include <cocos2d/cocos2d.h>
#endif

struct Screen
{
	/// Returns the width of the screen of the current device.
	static float width()
	{
	#if defined(ONWIRE_ANDROID)

		static float w = 0;

	#elif defined(ONWIRE_IOS)

		static float w = [CCDirector sharedDirector].winSize.width;

	#endif

		return w;
	}

	/// Returns the height of the screen of the current device.
	static float height()
	{
	#if defined(ONWIRE_ANDROID)

		static float h = 0;

	#elif defined(ONWIRE_IOS)

		static float h = [CCDirector sharedDirector].winSize.height;

	#endif

		return h;
	}

	/// Returns the DPI of the screen.
	static int dpi()
	{
		return 326;  // iPhone 4 DPI
	}
};

#endif
