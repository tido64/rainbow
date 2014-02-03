// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_TOUCH_H_
#define INPUT_TOUCH_H_

/// Structure for keeping track of touches.
struct Touch
{
	unsigned int hash;        ///< Unique hash value for each touch.
	int x, y;                 ///< Position of touch.
	int x0, y0;               ///< Previous position of touch.
	unsigned long timestamp;  ///< The time this event occurred in milliseconds.

	inline Touch();
	inline Touch(const unsigned int hash,
	             const int x,
	             const int y,
	             const unsigned long t);
	inline Touch(const unsigned int hash,
	             const int x,
	             const int y,
	             const int x0,
	             const int y0,
	             const unsigned long t);

	/// Two touch events are equal if their hash values are; i.e. they share a
	/// common origin.
	inline bool operator==(const Touch &t) const;
};

Touch::Touch() : hash(0), x(0), y(0), x0(0), y0(0), timestamp(0) { }

Touch::Touch(const unsigned int hash,
             const int x,
             const int y,
             const unsigned long t)
    : hash(hash), x(x), y(y), x0(x), y0(y), timestamp(t) { }

Touch::Touch(const unsigned int hash,
             const int x,
             const int y,
             const int x0,
             const int y0,
             const unsigned long t)
    : hash(hash), x(x), y(y), x0(x0), y0(y0), timestamp(t) { }

bool Touch::operator==(const Touch &t) const
{
	return this->hash == t.hash;
}

#endif
