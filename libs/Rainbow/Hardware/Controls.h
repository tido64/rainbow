/// Handles accelerometer/gyroscope and touch events independent of platform.

/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/Input/Touch.h>
#include <Rainbow/Touchable.h>
#include <Rainbow/Vector.h>

class Controls
{
public:
	static inline Controls* Instance()
	{
		static Controls c;
		return &c;
	}

	void accelerate(const float x, const float y);
	void add_touchable(Touchable *);

	const Touches* get_touches() const
	{
		return &touches;
	}

	void touch_began(const Touch *touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(const Touch *touches, const unsigned int count);
	void touch_moved(const Touch *touches, const unsigned int count);

private:
	Vector<Touchable *> touchables;
	Touches touches;

	Controls() { }
	Controls(const Controls &);
	Controls& operator=(const Controls &);
};

#endif
