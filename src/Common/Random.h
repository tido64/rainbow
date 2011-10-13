#ifndef RANDOM_H_
#define RANDOM_H_

#include <ctime>

#if defined(_MSC_VER) && (defined(_M_X64) || _M_IX86_FP >= 2)
#	define __SSE2__ 1
#elif defined(__clang__) || defined(__GNUC__)
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wlong-long"
#endif

#define DSFMT_MEXP 19937
#include <dSFMT/dSFMT.h>

#if defined(__clang__) || defined(__GNUC__)
#	pragma GCC diagnostic pop
#endif

/// Define floating point precision for pseudo-random generated numbers.
typedef float float_p;

class Random
{
public:
	static Random& Instance();

	float_p next();

	template<class T>
	T next(const T &n)
	{
		return static_cast<T>(this->next() * n);
	}

	template<class T>
	T next(const T &n1, const T &n2)
	{
		assert(n1 < n2 || !"Rainbow::Random::next: Parameters must be in ascending order");
		return static_cast<T>(this->next() * (n2 - n1)) + n1;
	}

private:
	Random(uint32_t seed = 0);
	Random(uint32_t init_key[], int key_length);

	/// Intentionally left undefined.
	Random(const Random &);

	/// Intentionally left undefined.
	Random& operator=(const Random &);
};

inline Random& Random::Instance()
{
	static Random rand;
	return rand;
}

inline float_p Random::next()
{
	return static_cast<float_p>(dsfmt_genrand_close1_open2(&dsfmt_global_data) - 1.0);
}

inline Random::Random(uint32_t init_key[], int key_length)
{
	dsfmt_init_by_array(&dsfmt_global_data, init_key, key_length);
}

#endif
