/// Provides algorithms and mathematical methods.
///
/// \see http://graphics.stanford.edu/~seander/bithacks.html
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef COMMON_ALGORITHM_H_
#define COMMON_ALGORITHM_H_

#include <cmath>
#include <cstdlib>
#include <limits>

#include "Common/Constants.h"

#ifndef _MSC_VER
#	define pure __attribute__((const))
#else
#	define pure
#	undef max
#	undef min
#endif

namespace Rainbow
{
	/// Converts radians to degrees.
	inline float degrees(const float r) pure;

	/// Compares two floating point numbers and approximate.
	/// \return \c true when approximately equal.
	template<typename T>
	inline bool equal(const T &a, const T &b) pure;

	/// Fast inverse square root by 0x5f3759df.
	inline float fast_invsqrt(float x) pure;

	/// Determines whether an integer is a power of 2.
	inline bool is_pow2(const unsigned int i) pure;

	/// Low-pass filter.
	inline float low_pass(const float value, const float low_pass) pure;

	template<typename T>
	inline const T& max(const T &a, const T &b) pure;

	template<typename T>
	inline const T& min(const T &a, const T &b) pure;

	/// Calculates the next power of 2.
	/// \note 0 is incorrectly considered a power of 2.
	inline unsigned int next_pow2(unsigned int i) pure;

	/// Converts degrees to radians.
	inline float radians(const float d) pure;

	template<typename T>
	inline void swap(T &a, T &b);

	/// Converts a UTF-8 character to UTF-32.
	/// \param[in]  str    UTF-8 encoded string.
	/// \param[out] bytes  Number of bytes consumed.
	/// \return UTF-32 character. Otherwise -1.
	inline unsigned long utf8_decode(const unsigned char *str, size_t &bytes);

	float degrees(const float r)
	{
		return r * kRadian;
	}

	template<typename T>
	bool equal(const T &a, const T &b)
	{
		return fabs(a - b) <= max(fabs(a), fabs(b)) * std::numeric_limits<T>::epsilon();
	}

	float fast_invsqrt(float x)
	{
		float xhalf = x * 0.5f;
		int i = *reinterpret_cast<int*>(&x);
		i = 0x5f3759df - (i >> 1);
		x = *reinterpret_cast<float*>(&i);
		return x * (1.5f - (xhalf * x * x));
	}

	bool is_pow2(const unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	float low_pass(const float value, const float low_pass)
	{
		return kLowPassAlpha * powf(10.0f, value * kLowPassAlpha) + (1.0f - kLowPassAlpha) * low_pass;
	}

	template<typename T>
	const T& max(const T &a, const T &b)
	{
		return (a < b) ? b : a;
	}

	template<typename T>
	const T& min(const T &a, const T &b)
	{
		return (b < a) ? b : a;
	}

	unsigned int next_pow2(unsigned int i)
	{
		--i;
		i |= i >>  1;
		i |= i >>  2;
		i |= i >>  4;
		i |= i >>  8;
		i |= i >> 16;
		return ++i;
	}

	float radians(const float d)
	{
		return d * kDegree;
	}

	template<typename T>
	void swap(T &a, T &b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	unsigned long utf8_decode(const unsigned char *str, size_t &bytes)
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

#undef pure
#endif
