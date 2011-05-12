#ifndef TOUCHABLE_H_
#define TOUCHABLE_H_

#include "Input/Touch.h"

/// Interface for touchable objects.
///
/// This interface can be used for both touch and mouse inputs.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Touchable
{
public:
	virtual void touch_began(const Touch *const touches, const unsigned int count) = 0;
	virtual void touch_canceled() = 0;
	virtual void touch_ended(const Touch *const touches, const unsigned int count) = 0;
	virtual void touch_moved(const Touch *const touches, const unsigned int count) = 0;
};

#endif
