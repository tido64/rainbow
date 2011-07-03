#ifndef TOUCH_H_
#define TOUCH_H_

#include <functional>

#include "Common/Vec2.h"
#include "Hardware/Screen.h"

#if defined(RAINBOW_IOS)
#	include <UIKit/UITouch.h>
#endif

/// Data structures for keeping track of touches.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
struct Touch
{
	unsigned int hash;  ///< Unique hash value that distinguishes a touch from another
	Vec2i initial;      ///< The initial x- and y-values
	Vec2i position;     ///< Position of touch

	Touch() : hash(0) { }

	Touch& operator=(const Touch &t)
	{
		if (this == &t)
			return *this;
		if (this->hash != t.hash)
		{
			this->hash = t.hash;
			this->initial = t.position;
		}
		this->position = t.position;
		return *this;
	}

#if defined(RAINBOW_IOS)

	/// Only available on iOS. Used for converting a UITouch.
	Touch& operator=(const UITouch *const t)
	{
		if (t.hash == this->hash)
			return *this;
		CGPoint p = [t locationInView:[t view]];
		this->position.x = p.x;
		this->position.y = p.y;
		if (this->hash == 0)
		{
			this->hash = t.hash;
			this->initial = this->position;
		}
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
