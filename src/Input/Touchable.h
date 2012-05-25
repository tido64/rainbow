#ifndef TOUCHABLE_H_
#define TOUCHABLE_H_

#include "Input/Touch.h"

/// Interface for touchable objects.
///
/// This interface can be used for both touch and mouse inputs.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Touchable
{
public:
	virtual ~Touchable() { }

	/// User puts finger down on the device.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	virtual void touch_began(const Touch *const touches, const size_t count) = 0;

	/// Canceled touch event. May occur when the user moves finger off-screen
	/// or the application got interrupted by external events.
	virtual void touch_canceled() = 0;

	/// User lifts finger.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	virtual void touch_ended(const Touch *const touches, const size_t count) = 0;

	/// User moves finger around while it still is on the device.
	/// \param touches  The id and position of each touch
	/// \param count    Number of touches
	virtual void touch_moved(const Touch *const touches, const size_t count) = 0;
};

#endif
