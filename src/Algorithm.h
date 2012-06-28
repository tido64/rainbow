/// Provides algorithms and mathematical functions.
///
/// \see http://graphics.stanford.edu/~seander/bithacks.html
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef ALGORITHM_H_
#define ALGORITHM_H_

#include <cfloat>
#include <cmath>
#include <cstdlib>

#include "Common/Constants.h"

namespace Rainbow
{
	/// Angle between two points with bearing north.
	inline float angle(const float a_x, const float a_y, const float b_x, const float b_y)
	{
		return atanf((b_y - a_y) / (b_x - a_x));
	}

	/// Convert radians to degrees.
	inline float degrees(const float r)
	{
		return r * kRadian;
	}

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
	/// \return \c true when approximately equal.
	inline bool equalf(const float a, const float b)
	{
		return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * FLT_EPSILON);
	}

	/// Determine whether an integer is a power of 2.
	inline bool is_pow2(const unsigned int i)
	{
		return i && !(i & (i - 1));
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

	template<class T>
	inline const T& min(const T &a, const T &b)
	{
		return (a < b) ? a : b;
	}

	/// Calculate the next power of 2.
	/// \note 0 is incorrectly considered a power of 2.
	inline unsigned int next_pow2(unsigned int i)
	{
		--i;
		i |= i >>  1;
		i |= i >>  2;
		i |= i >>  4;
		i |= i >>  8;
		i |= i >> 16;
		return ++i;
	}

	/// Convert degrees to radians.
	inline float radians(const float d)
	{
		return d * kDegree;
	}

	/// Convert a UTF-8 character to UTF-32.
	/// \param[in]  str    UTF-8 encoded string.
	/// \param[out] bytes  Number of bytes consumed.
	/// \return UTF-32 character. Otherwise -1.
	inline unsigned long utf8_decode(const unsigned char *str, size_t &bytes)
	{
		bytes = 0;
		if (!(str[0] & 0x80))
		{
			bytes = 1;
			return str[0];
		}
		else if ((str[0] & 0xe0) == 0xc0)
		{
			if ((str[1] & 0xc0) != 0x80)
				return -1;

			bytes = 2;
			unsigned long ucs = str[0] & 0x1f;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			return ucs;
		}
		else if ((str[0] & 0xf0) == 0xe0)
		{
			if ((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80)
				return -1;

			bytes = 3;
			unsigned long ucs = str[0] & 0x0f;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			ucs <<= 6;
			ucs |= str[2] & 0x3f;
			return ucs;
		}
		else if ((str[0] & 0xf8) == 0xf0)
		{
			if ((str[1] & 0xc0) != 0x80 || (str[2] & 0xc0) != 0x80 || (str[3] & 0xc0) != 0x80)
				return -1;

			bytes = 4;
			unsigned long ucs = str[0] & 0x0e;
			ucs <<= 6;
			ucs |= str[1] & 0x3f;
			ucs <<= 6;
			ucs |= str[2] & 0x3f;
			ucs <<= 6;
			ucs |= str[3] & 0x3f;
			return ucs;
		}
		else
			return -1;
	}
}

#endif
