#ifdef RAINBOW_ACCELERATED

#ifndef ACCELERATION_H_
#define ACCELERATION_H_

#ifdef RAINBOW_IOS
#	include <UIKit/UIAccelerometer.h>
#endif

#define RAINBOW_FIXED_SCALE      1000.0f
#define RAINBOW_INV_FIXED_SCALE  (1.0f / RAINBOW_FIXED_SCALE)


namespace Rainbow
{
	/// Structure for handling accelerometer data.

	/// Uses fixed-point arithmetic with a scaling factor of 1/1000.
	///
	/// Copyright 2011 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	class Acceleration
	{
	public:
		int x, y, z;
		double timestamp;

		Acceleration() :
			x(0), y(0), z(0), timestamp(0.0) { }

		Acceleration(const Acceleration &a) :
			x(cut(a.x)), y(cut(a.y)), z(cut(a.z)), timestamp(a.timestamp) { }

	#ifdef RAINBOW_IOS

		/// Only available on iOS. Used for converting a UIAcceleration.
		Acceleration& operator=(const UIAcceleration *const a)
		{
			this->x = this->cut(a.x);
			this->y = this->cut(a.y);
			this->z = this->cut(a.z);
			this->timestamp = a.timestamp;
			return *this;
		}

	#endif

	private:
		inline int cut(const float x)
		{
			return static_cast<int>(x * RAINBOW_FIXED_SCALE);
		}
	};
}

typedef Rainbow::Acceleration Acceleration;

#endif

#endif
