/// Retrieves screen size regardless of platform.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef SCREEN_H_
#define SCREEN_H_

#include "Platform.h"

class Screen
{
public:

	/// Screen is a singleton.
	static Screen *Instance();

	/// Returns the width of the screen of the current device.
	inline unsigned int get_width() { return this->width; }

	/// Returns the height of the screen of the current device.
	inline unsigned int get_height() { return this->height; }

private:
	unsigned int
		width,	///< Width of the screen of the current device
		height; ///< Height of the screen of the current device
	Screen();
};

#endif
