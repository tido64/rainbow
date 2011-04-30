#ifndef INPUT_H_
#define INPUT_H_

#include "../Common/Vector.h"
#include "../Platform.h"
#include "../Touchable.h"
#include "Acceleration.h"


/// Handles accelerometer/gyroscope and touch events independent of platform.

/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Input
{
public:
	static Input& Instance()
	{
		static Input c;
		return c;
	}

	mutable bool touched;

#ifdef RAINBOW_ACCELERATED

	bool accelerated;
	Acceleration acceleration;

#endif

	const Touches& get_touches() const;
	void touch_began(const Touch *touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(const Touch *touches, const unsigned int count);
	void touch_moved(const Touch *touches, const unsigned int count);

private:
	Touchable *director;
	Touches touches;

	Input() { }

	/// Intentionally left undefined.
	Input(const Input &);

	/// Intentionally left undefined.
	Input& operator=(const Input &);

	friend class Director;
};

inline const Touches& Input::get_touches() const
{
	this->touched = false;
	return touches;
}

#endif
