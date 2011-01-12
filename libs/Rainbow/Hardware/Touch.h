/// Data structures for keeping track of touches.

/// \see http://www.boost.org/doc/libs/1_45_0/doc/html/unordered.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef TOUCH_H_
#define TOUCH_H_

#include <functional>

#include <boost/unordered_set.hpp>
#include <Rainbow/Hardware/Screen.h>

#if defined(RAINBOW_IOS)
#	include <UIKit/UITouch.h>
#endif

namespace Rainbow
{
	struct Touch
	{
		int x;              ///< x-component of the touch location
		int y;              ///< y-component of the touch location
		unsigned int hash;  ///< Unique hash value that distinguishes a touch from another

		Touch() : x(0.0f), y(0.0f), hash(0) { }
		Touch(const Touch *t) : x(t->x), y(t->y), hash(t->hash) { }

	#if defined(RAINBOW_IOS)

		Touch(const UITouch *t) : hash(t.hash)
		{
			CGPoint p = [t locationInView:[t view]];
			this->x = p.x;
			this->y = Screen::Instance()->height() - p.y;
		}

		Touch& operator=(const UITouch *t)
		{
			CGPoint p = [t locationInView:[t view]];
			this->x = p.x;
			this->y = Screen::Instance()->height() - p.y;
			this->hash = t.hash;
			return *this;
		}

	#endif
	};

#if defined(RAINBOW_IOS)

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

#endif

	/// Determines whether two touches are from the same finger.
	struct TouchEqual : std::binary_function<Touch, Touch, bool>
	{
		bool operator()(const Touch &a, const Touch &b) const
		{
			return a.hash == b.hash;
		}
	};

	/// Touches already have a hash value assigned to them that we can use directly.
	struct TouchHasher : std::unary_function<Touch, unsigned int>
	{
		unsigned int operator()(const Touch &t) const
		{
			return t.hash;
		}
	};
}

typedef Rainbow::Touch Touch;
typedef boost::unordered_set<Touch, Rainbow::TouchHasher, Rainbow::TouchEqual> Touches;

#endif
