/// Provides algorithms and mathematical functions.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cfloat>
#include <cmath>

#include "Common/Constants.h"

namespace Rainbow
{
	/// Angle between two points with bearing north.
	inline float angle(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		return atanf((b_y - a_y) / (b_x - a_x));
	}

	/// Convert degrees to radians.
	inline float deg2rad(const float d) { return d * kPi_180; }

	/// Calculate the distance between two points.
	inline float distance(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		float s1 = b_x - a_x;
		float s2 = b_y - a_y;
		s1 *= s1;
		s2 *= s2;
		return sqrt(s1 + s2);
	}

	/// Compare two floating point numbers and approximate.
	/// \return True when approximately equal
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	/// Convert an integer to a C-string.
	inline char* itoa(int i)
	{
		int sz = 1;
		if (i < 0)
		{
			++sz;
			i *= -1;
		}
		int tmp = i;
		while (tmp > 9)
		{
			tmp /= 10;
			++sz;
		}

		char *str = new char[sz + 1];
		str[sz] = '\0';
		for (int j = sz - 1; j > 0; --j)
		{
			tmp = i % 10;
			str[j] = tmp + '0';
			i /= 10;
		}
		str[0] = (!i) ? '-' : i + '0';
		return str;
	}

	/// Convert radians to degrees.
	inline float rad2deg(const float r) { return r * k180_Pi; }
}

#endif
