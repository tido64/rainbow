/// Structure for handling accelerometer data.

/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifdef RAINBOW_ACCELERATED

#ifndef ACCELERATION_H_
#define ACCELERATION_H_

#ifdef RAINBOW_IOS
#	include <UIKit/UIAccelerometer.h>
#endif

class Acceleration
{
public:
	float x, y, z;
	double timestamp;

	Acceleration() : x(0.0f), y(0.0f), z(0.0f), timestamp(0.0) { }

#ifdef RAINBOW_IOS

	/// Only available on iOS. Used for converting a UIAcceleration.
	Acceleration(UIAcceleration *a) : x(a.x), y(a.y), z(a.z), timestamp(a.timestamp) { }

	/// Only available on iOS. Used for converting a UIAcceleration.
	Acceleration& operator=(const UIAcceleration *a)
	{
		this->x = a.x;
		this->y = a.y;
		this->z = a.z;
		this->timestamp = a.timestamp;
		return *this;
	}

#endif
};

#endif

#endif
