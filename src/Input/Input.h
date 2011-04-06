/// Handles accelerometer/gyroscope and touch events independent of platform.

/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef INPUT_H_
#define INPUT_H_

#include "../Platform.h"
#include "../Vector.h"

#ifdef RAINBOW_ACCELERATED
#	include "Acceleration.h"
#endif

#ifdef RAINBOW_TOUCHED
#	include "../Touchable.h"
#endif

class Input
{
public:
	static inline Input& Instance()
	{
		static Input c;
		return c;
	}

#ifdef RAINBOW_ACCELERATED

	bool accelerated;
	Acceleration acceleration;

#endif

#ifdef RAINBOW_TOUCHED

	mutable bool touched;

	inline const Touches& get_touches() const
	{
		this->touched = false;
		return touches;
	}

	void touch_began(const Touch *touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(const Touch *touches, const unsigned int count);
	void touch_moved(const Touch *touches, const unsigned int count);

#endif

private:
#ifdef RAINBOW_TOUCHED
	Touches touches;
#endif

	Input() { }
	Input(const Input &);
	Input& operator=(const Input &);
};

#endif
