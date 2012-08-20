#ifndef RANDOM_H_
#define RANDOM_H_

#include <ctime>

#include "Common/Debug.h"

#if defined(RAINBOW_UNIX)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif

#define DSFMT_MEXP 19937
#ifndef RAINBOW_IOS  // Workaround Xcode's super inclusion algorithm
#	include <dSFMT/dSFMT.h>
#else
#	include "dSFMT.h"
#endif

#if defined(RAINBOW_UNIX)
#	pragma GCC diagnostic pop
#endif

/// Define floating point precision for pseudo-random generated numbers.
typedef float float_p;

/// C++ wrapper for dSFMT random number generator.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Random
{
public:
	static inline Random& Instance();

	inline float_p next();

	template<class T>
	T next(const T &n)
	{
		return static_cast<T>(this->next() * n);
	}

	template<class T>
	T next(const T &n1, const T &n2)
	{
		R_ASSERT(n1 < n2, "Parameters must be in ascending order");
		return static_cast<T>(this->next() * (n2 - n1)) + n1;
	}

private:
	inline Random(uint32_t seed = 0);
	inline Random(uint32_t init_key[], int key_length);

	/// Intentionally left undefined.
	Random(const Random &);

	/// Intentionally left undefined.
	Random& operator=(const Random &);
};

Random& Random::Instance()
{
	static Random rand;
	return rand;
}

float_p Random::next()
{
	return static_cast<float_p>(dsfmt_genrand_close1_open2(&dsfmt_global_data) - 1.0);
}

Random::Random(uint32_t seed)
{
	if (!seed)
		seed = static_cast<uint32_t>(time(nullptr));
	dsfmt_init_gen_rand(&dsfmt_global_data, seed);
}

Random::Random(uint32_t init_key[], int key_length)
{
	dsfmt_init_by_array(&dsfmt_global_data, init_key, key_length);
}

#endif
