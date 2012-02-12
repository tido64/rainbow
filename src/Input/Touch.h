#ifndef TOUCH_H_
#define TOUCH_H_

#include "Platform.h"
#include "Common/Vec2.h"

/// Structure for keeping track of touches.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
struct Touch
{
	unsigned int hash;  ///< Unique hash value for each touch.
	int x, y;           ///< Position of touch.

	Touch() : hash(0), x(0), y(0) { }
	Touch(const int x, const int y) : hash(0), x(x), y(y) { }
	Touch(const unsigned hash, const int x, const int y) :
		hash(hash), x(x), y(y) { }
};

#endif
