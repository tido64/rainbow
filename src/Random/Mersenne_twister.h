#ifndef MERSENNE_TWISTER_H_
#	define MERSENNE_TWISTER_H_
#	if defined(_MSC_VER) && (defined(_M_X64) || _M_IX86_FP >= 2)
#		define __SSE2__
#	endif

#include <cstring>
#include <ctime>

#include "dSFMT-params.h"

/// Double precision SIMD-oriented Fast Mersenne Twister (dSFMT) based on IEEE 754 format.
///
/// \author Mutsuo Saito (Hiroshima University)
/// \author Makoto Matsumoto (Hiroshima University)
///
/// Copyright (C) 2007,2008 Mutsuo Saito, Makoto Matsumoto and Hiroshima
/// University. All rights reserved.
///
/// The new BSD License is applied to this software
/// (see http://www.opensource.org/licenses/bsd-license.php)
class Random
{
public:
	static Random& Instance()
	{
		static Random inst;
		return inst;
	}

	double next()
	{
		double *psfmt64 = &this->dsfmt.status[0].d[0];
		if (this->dsfmt.idx >= DSFMT_N64)
		{
			w128_t lung = this->dsfmt.status[DSFMT_N];
			do_recursion(&this->dsfmt.status[0], &this->dsfmt.status[0], &this->dsfmt.status[DSFMT_POS1], &lung);
			int i = 1;
			for (; i < DSFMT_N - DSFMT_POS1; ++i)
				do_recursion(&this->dsfmt.status[i], &this->dsfmt.status[i], &this->dsfmt.status[i + DSFMT_POS1], &lung);
			for (; i < DSFMT_N; ++i)
				do_recursion(&this->dsfmt.status[i], &this->dsfmt.status[i], &this->dsfmt.status[i + DSFMT_POS1 - DSFMT_N], &lung);
			this->dsfmt.status[DSFMT_N] = lung;
			this->dsfmt.idx = 0;
		}
		return psfmt64[this->dsfmt.idx++] - 1.0;
	}

	inline double next(double min, double max)
	{
		return this->next() * (max - min) + min;
	}

	inline int next_int(int i)
	{
		return static_cast<int>(this->next() * i);
	}

	inline int next_int(int min, int max)
	{
		return static_cast<int>(this->next() * (max - min)) + min;
	}

	/// This function initializes the internal state array with a 32-bit
	/// integer seed.
	/// \param seed  a 32-bit integer used as the seed
	void seed(uint32_t seed)
	{
		unsigned int *psfmt = &this->dsfmt.status[0].u32[0];
		psfmt[idxof(0)] = seed;

		for (int i = 1; i < (DSFMT_N + 1) * 4; ++i)
			psfmt[idxof(i)] = 1812433253UL * (psfmt[idxof(i - 1)] ^ (psfmt[idxof(i - 1)] >> 30)) + i;

		this->initial_mask();
		this->period_certification();
		this->dsfmt.idx = DSFMT_N64;
	}

	/// This function initializes the internal state array with a 32-bit
	/// integer seed.
	/// \param init_key    the array of 32-bit integers, used as a seed
	/// \param key_length  the length of init_key
	void seed(uint32_t init_key[], int key_length)
	{
		int lag;
		int mid;
		int size = (DSFMT_N + 1) * 4;  // pulmonary

		if (size >= 623)
			lag = 11;
		else if (size >= 68)
			lag = 7;
		else if (size >= 39)
			lag = 5;
		else
			lag = 3;
		mid = (size - lag) / 2;

		uint32_t *psfmt32 = &this->dsfmt.status[0].u32[0];
		memset(this->dsfmt.status, 0x8b, sizeof(this->dsfmt.status));
		int count = (key_length + 1 > size) ? key_length + 1 : size;
		uint32_t r = ini_func1(psfmt32[idxof(0)]
			^ psfmt32[idxof(mid % size)]
			^ psfmt32[idxof((size - 1) % size)]);
		psfmt32[idxof(mid % size)] += r;
		r += key_length;
		psfmt32[idxof((mid + lag) % size)] += r;
		psfmt32[idxof(0)] = r;
		--count;

		int i = 1, j = 0;
		for (; (j < count) && (j < key_length); ++j)
		{
			r = ini_func1(psfmt32[idxof(i)]
				^ psfmt32[idxof((i + mid) % size)]
				^ psfmt32[idxof((i + size - 1) % size)]);
			psfmt32[idxof((i + mid) % size)] += r;
			r += init_key[j] + i;
			psfmt32[idxof((i + mid + lag) % size)] += r;
			psfmt32[idxof(i)] = r;
			i = (i + 1) % size;
		}
		for (; j < count; ++j)
		{
			r = ini_func1(psfmt32[idxof(i)]
				^ psfmt32[idxof((i + mid) % size)]
				^ psfmt32[idxof((i + size - 1) % size)]);
			psfmt32[idxof((i + mid) % size)] += r;
			r += i;
			psfmt32[idxof((i + mid + lag) % size)] += r;
			psfmt32[idxof(i)] = r;
			i = (i + 1) % size;
		}
		for (j = 0; j < size; ++j)
		{
			r = ini_func2(psfmt32[idxof(i)]
				+ psfmt32[idxof((i + mid) % size)]
				+ psfmt32[idxof((i + size - 1) % size)]);
			psfmt32[idxof((i + mid) % size)] ^= r;
			r -= i;
			psfmt32[idxof((i + mid + lag) % size)] ^= r;
			psfmt32[idxof(i)] = r;
			i = (i + 1) % size;
		}

		this->initial_mask();
		this->period_certification();
		this->dsfmt.idx = DSFMT_N64;
	}

	inline double operator()()
	{
		return this->next();
	}

	inline unsigned int operator()(unsigned int i)
	{
		return static_cast<unsigned int>(this->next() * i);
	}

private:
	dsfmt_t dsfmt;     ///< dsfmt internal state vector

	Random() { this->seed(static_cast<uint32_t>(time(0))); }
	Random(uint32_t seed) { this->seed(seed); }
	Random(uint32_t init_key[], int key_length) { this->seed(init_key, key_length); }
	Random(const Random &);

#if defined(__SSE2__)

	/// This function represents the recursion formula.
	/// \param r  output 128-bit
	/// \param a  a 128-bit part of the internal state array
	/// \param b  a 128-bit part of the internal state array
	/// \param d  a 128-bit part of the internal state array (I/O)
	inline void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *u)
	{
		// mask data for sse2
		static const __m128i sse2_param_mask = _mm_set_epi32(DSFMT_MSK32_3, DSFMT_MSK32_4, DSFMT_MSK32_1, DSFMT_MSK32_2);

		__m128i v, w, x, y, z;

		x = a->si;
		z = _mm_slli_epi64(x, DSFMT_SL1);
		y = _mm_shuffle_epi32(u->si, SSE2_SHUFF);
		z = _mm_xor_si128(z, b->si);
		y = _mm_xor_si128(y, z);

		v = _mm_srli_epi64(y, DSFMT_SR);
		w = _mm_and_si128(y, sse2_param_mask);
		v = _mm_xor_si128(v, x);
		v = _mm_xor_si128(v, w);
		r->si = v;
		u->si = y;
	}

#elif defined(__ALTIVEC__)

	/// This function represents the recursion formula.
	/// \param r     output
	/// \param a     a 128-bit part of the internal state array
	/// \param b     a 128-bit part of the internal state array
	/// \param lung  a 128-bit part of the internal state array
	inline static void do_recursion(w128_t *r, w128_t *a, w128_t * b, w128_t *lung)
	{
		const vector unsigned char sl1 = ALTI_SL1;
		const vector unsigned char sl1_perm = ALTI_SL1_PERM;
		const vector unsigned int sl1_msk = ALTI_SL1_MSK;
		const vector unsigned char sr1 = ALTI_SR;
		const vector unsigned char sr1_perm = ALTI_SR_PERM;
		const vector unsigned int sr1_msk = ALTI_SR_MSK;
		const vector unsigned char perm = ALTI_PERM;
		const vector unsigned int msk1 = ALTI_MSK;
		vector unsigned int w, x, y, z;

		z = a->s;
		w = lung->s;
		x = vec_perm(w, (vector unsigned int)perm, perm);
		y = vec_perm(z, sl1_perm, sl1_perm);
		y = vec_sll(y, sl1);
		y = vec_and(y, sl1_msk);
		w = vec_xor(x, b->s);
		w = vec_xor(w, y);
		x = vec_perm(w, (vector unsigned int)sr1_perm, sr1_perm);
		x = vec_srl(x, sr1);
		x = vec_and(x, sr1_msk);
		y = vec_and(w, msk1);
		z = vec_xor(z, y);
		r->s = vec_xor(z, x);
		lung->s = w;
	}

#else  // plain C

	/// This function represents the recursion formula.
	/// \param r     output 128-bit
	/// \param a     a 128-bit part of the internal state array
	/// \param b     a 128-bit part of the internal state array
	/// \param lung  a 128-bit part of the internal state array (I/O)
	inline static void do_recursion(w128_t *r, w128_t *a, w128_t * b, w128_t *lung)
	{
		uint64_t t0, t1, L0, L1;

		t0 = a->u[0];
		t1 = a->u[1];
		L0 = lung->u[0];
		L1 = lung->u[1];
		lung->u[0] = (t0 << DSFMT_SL1) ^ (L1 >> 32) ^ (L1 << 32) ^ b->u[0];
		lung->u[1] = (t1 << DSFMT_SL1) ^ (L0 >> 32) ^ (L0 << 32) ^ b->u[1];
		r->u[0] = (lung->u[0] >> DSFMT_SR) ^ (lung->u[0] & DSFMT_MSK1) ^ t0;
		r->u[1] = (lung->u[1] >> DSFMT_SR) ^ (lung->u[1] & DSFMT_MSK2) ^ t1;
	}

#endif

	/// This function represents a function used in the initialization
	/// by init_by_array
	/// \param x  32-bit integer
	/// \return 32-bit integer
	uint32_t ini_func1(uint32_t x)
	{
		return (x ^ (x >> 27)) * (uint32_t)1664525UL;
	}

	/// This function represents a function used in the initialization
	/// by init_by_array
	/// \param x  32-bit integer
	/// \return 32-bit integer
	uint32_t ini_func2(uint32_t x)
	{
		return (x ^ (x >> 27)) * (uint32_t)1566083941UL;
	}

	/// This function initializes the internal state array to fit the
	/// IEEE754 format.
	void initial_mask()
	{
		uint64_t *psfmt = &this->dsfmt.status[0].u[0];
		for (int i = 0; i < DSFMT_N * 2; ++i)
			psfmt[i] = (psfmt[i] & DSFMT_LOW_MASK) | DSFMT_HIGH_CONST;
	}

	/// This function certificate the period of 2^{SFMT_MEXP}-1.
	void period_certification()
	{
		uint64_t pcv[2] = {DSFMT_PCV1, DSFMT_PCV2};
		uint64_t tmp[2];
		uint64_t inner;

		tmp[0] = (this->dsfmt.status[DSFMT_N].u[0] ^ DSFMT_FIX1);
		tmp[1] = (this->dsfmt.status[DSFMT_N].u[1] ^ DSFMT_FIX2);

		inner = tmp[0] & pcv[0];
		inner ^= tmp[1] & pcv[1];
		int i = 32;
		for (; i > 0; i >>= 1)
			inner ^= inner >> i;
		inner &= 1;
		// check OK
		if (inner == 1)
			return;

		// check NG, and modification
	#if (DSFMT_PCV2 & 1) == 1
		this->dsfmt.status[DSFMT_N].u[1] ^= 1;
	#else
		int j;
		uint64_t work;
		for (i = 1; i >= 0; i--)
		{
			work = 1;
			for (j = 0; j < 64; ++j)
			{
				if ((work & pcv[i]) != 0)
				{
					this->dsfmt.status[DSFMT_N].u[i] ^= work;
					return;
				}
				work = work << 1;
			}
		}
	#endif
		return;
	}

	/// This function simulate a 32-bit array index overlapped to 64-bit
	/// array of LITTLE ENDIAN in BIG ENDIAN machine.
	inline static int idxof(int i)
	{
	#ifdef DSFMT_BIG_ENDIAN
		return i ^ 1;
	#else
		return i;
	#endif
	}

	Random& operator=(const Random &);
};

#endif
