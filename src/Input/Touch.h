#ifndef TOUCH_H_
#define TOUCH_H_

#include <functional>

#include "Platform.h"
#include "Common/Vec2.h"

#if defined(RAINBOW_IOS)
#	include <UIKit/UITouch.h>
#endif

/// Structure for keeping track of touches.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct Touch
{
	unsigned int hash;  ///< Unique hash value that distinguishes a touch from another
	int x, y;           ///< Position of touch

	Touch() : hash(0), x(0), y(0) { }
	Touch(const int x, const int y) : hash(0), x(x), y(y) { }
	Touch(const unsigned hash, const int x, const int y) : hash(hash), x(x), y(y) { }

#if defined(RAINBOW_IOS)

	/// Only available on iOS.
	void operator*=(const int f)
	{
		this->x *= f;
		this->y *= f;
	}

	/// Only available on iOS. Used for converting a UITouch.
	Touch& operator=(const UITouch *const t)
	{
		if (t.hash == this->hash)
			return *this;
		this->hash = t.hash;
		CGPoint p = [t locationInView:[t view]];
		this->x = p.x;
		this->y = p.y;
		return *this;
	}

#endif
};

#if defined(RAINBOW_IOS)

namespace Rainbow
{
	/// Only available on iOS. Used for converting an NSSet to a C array.
	struct TouchArray : std::unary_function<NSSet *&, Touch*>
	{
		Touch* operator()(NSSet *&touches) const
		{
			Touch *t = new Touch[touches.count];
			Touch *t_iter = t;
			for (UITouch *touch in touches)
			{
				*t_iter = touch;
				++t_iter;
			}
			return t;
		}
	};
}

#endif

#endif
