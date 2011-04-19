/// Data structures for keeping track of touches.

/// \see http://www.boost.org/doc/libs/1_46_1/doc/html/unordered.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef TOUCH_H_
#define TOUCH_H_

#include <functional>

#include <boost/unordered_map.hpp>

#include "../Common/Vec2.h"
#include "../Hardware/Screen.h"

#if defined(RAINBOW_IOS)
#	include <UIKit/UITouch.h>
#endif

struct Touch
{
	unsigned int hash;  ///< Unique hash value that distinguishes a touch from another
	Vec2i initial;      ///< The initial x- and y-values
	Vec2i position;     ///< Position of touch

	Touch() : hash(0) { }

	Touch& operator=(const Touch &t)
	{
		if (this->hash == 0)
		{
			this->hash = t.hash;
			this->initial = t.position;
		}
		this->position = t.position;
		return *this;
	}

#if defined(RAINBOW_IOS)

	/// Only available on iOS. Used for converting a UITouch.
	Touch& operator=(const UITouch *t)
	{
		CGPoint p = [t locationInView:[t view]];
		this->position.x = p.x;
		this->position.y = Screen::Instance().height() - p.y;
		if (this->hash == 0)
		{
			this->hash = t.hash;
			this->initial = this->position;
		}
		return *this;
	}

#endif
};

namespace Rainbow
{
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

	/// Determine whether two touches are from the same finger.
	struct TouchEqual : std::binary_function<unsigned int, unsigned int, bool>
	{
		inline bool operator()(const unsigned int &a, const unsigned int &b) const
		{
			return a == b;
		}
	};

	/// Touches already have a hash value assigned to them that we can use directly.
	struct TouchHasher : std::unary_function<unsigned int, std::size_t>
	{
		inline std::size_t operator()(const unsigned int &k) const
		{
			return static_cast<std::size_t>(k);
		}
	};
}

typedef boost::unordered_map<unsigned int, Touch, Rainbow::TouchHasher, Rainbow::TouchEqual> Touches;

#endif
