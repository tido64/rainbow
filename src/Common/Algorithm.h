// Provides algorithms and mathematical methods.
//
// \see http://graphics.stanford.edu/~seander/bithacks.html
//
// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_ALGORITHM_H_
#define COMMON_ALGORITHM_H_

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include "Common/Constants.h"
#include "Common/Constraints.h"

namespace rainbow
{
	template<typename T, size_t N>
	size_t array_size(const T (&)[N])
	{
		return N;
	}

	template<typename T>
	T clamp(const T x, const T min_val, const T max_val)
	{
		return std::min(std::max(x, min_val), max_val);
	}

	/// Converts radians to degrees.
	inline float degrees(const float r)
	{
		return r * static_cast<float>(kRadian);
	}

	/// Fast inverse square root by 0x5f3759df.
	inline float fast_invsqrt(float x)
	{
		float xhalf = x * 0.5f;
#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
		int i = *reinterpret_cast<int*>(&x);
		i = 0x5f3759df - (i >> 1);
		x = *reinterpret_cast<float*>(&i);
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif
		return x * (1.5f - (xhalf * x * x));
	}

	/// Compares two floating point numbers and approximate their equality.
	/// \return \c true when approximately equal.
	template<typename T, typename = FloatingPoint<T>>
	bool is_equal(const T a, const T b)
	{
		return fabs(a - b) <=
		       fmax(fabs(a), fabs(b)) * std::numeric_limits<T>::epsilon();
	}

	/// Determines whether an integer is a power of 2.
	inline bool is_pow2(const unsigned int i)
	{
		return i && !(i & (i - 1));
	}

	/// Low-pass filter.
	inline float low_pass(const float value, const float low_pass)
	{
		return kLowPassAlpha * powf(10.0f, value * kLowPassAlpha) +
		       (1.0f - kLowPassAlpha) * low_pass;
	}

	/// Calculates the next power of 2.
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

	/// Removes the element at \p pos by swapping it with the last element in
	/// \p container and popping it.
	template<typename T>
	void quick_erase(T &container, typename T::iterator pos)
	{
		std::swap(*pos, container.back());
		container.pop_back();
	}

	/// Converts degrees to radians.
	inline float radians(const float d)
	{
		return d * static_cast<float>(kDegree);
	}

	/// Removes the first element equal to \p v.
	template<typename T>
	typename T::iterator remove(T &container, const typename T::value_type &val)
	{
		auto i = std::find(std::begin(container), std::end(container), val);
		return (i == std::end(container)) ? i : container.erase(i);
	}

	/// Removes all elements that satisfy \p predicate and returns a
	/// past-the-end iterator for the new end of the range.
	template<typename T, typename F>
	typename T::iterator remove_if(T &container, F&& predicate)
	{
		return container.erase(std::remove_if(std::begin(container),
		                                      std::end(container),
		                                      std::forward<F>(predicate)),
		                       std::end(container));
	}

	/// Returns -1, 0, or 1 for \p x < 0, \p x == 0, or \p x > 0 respectively.
	template<typename T>
	T signum(const T x)
	{
		return (x > T(0)) - (x < T(0));
	}
}

#endif
