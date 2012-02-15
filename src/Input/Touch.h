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
	int x0, y0;         ///< Previous position of touch.

	inline Touch();
	inline Touch(const int x, const int y);
	inline Touch(const int x, const int y, const int x0, const int y0);

	/// Two touch objects are equal if their hash values are, i.e. they came
	/// from the same touch event.
	inline bool operator==(const Touch &t) const;
};

Touch::Touch() : hash(0), x(0), y(0), x0(0), y0(0) { }

Touch::Touch(const int x, const int y) :
	hash(0), x(x), y(y), x0(x), y0(y) { }

Touch::Touch(const int x, const int y, const int x0, const int y0) :
	hash(0), x(x), y(y), x0(x0), y0(y0) { }

bool Touch::operator==(const Touch &t) const
{
	return this->hash == t.hash;
}

#endif
