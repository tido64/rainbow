/// Handles accelerometer/gyroscope and touch events independent of platform.

/// \see http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef CONTROLS_H_
#define CONTROLS_H_

#include "../Framework/Sprite.h"
#include "../Framework/Touch.h"
#include "../Hardware/Screen.h"

class Controls
{
public:
	Controls();
	~Controls();

	void accelerate(const float x, const float y);
	void add_object(Sprite *);

	void touch_began(Touch *touches, const unsigned int count);
	void touch_canceled();
	void touch_ended(Touch *touches, const unsigned int count);
	void touch_moved(Touch *touches, const unsigned int count);

private:
	const unsigned int scr_w, scr_h, speed;
	unsigned int sprite_count;
	Sprite **sprites;
	Touches touches;
};

#endif
