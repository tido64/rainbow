// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_RANDOM_H_
#define COMMON_RANDOM_H_

#include <cstdint>
#include <random>

#include "Common/Constraints.h"
#include "Common/Logging.h"

// xorshift1024* + splitmix64
//
// Written in 2014-2015 by Sebastiano Vigna (vigna@acm.org)
//
// To the extent possible under law, the author has dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// See <http://creativecommons.org/publicdomain/zero/1.0/>.
//
// Source: <http://xorshift.di.unimi.it/>
namespace xorshift
{
    // This is a fast, top-quality generator. If 1024 bits of state are too
    // much, try a xoroshiro128+ generator.
    //
    // The state must be seeded so that it is not everywhere zero. If you have
    // a 64-bit seed, we suggest to seed a splitmix64 generator and use its
    // output to fill s.
    class xorshift1024star
    {
    public:
        static constexpr auto max()
        {
            return std::numeric_limits<uint64_t>::max();
        }

        static constexpr auto min()
        {
            return std::numeric_limits<uint64_t>::min();
        }

        // This is the jump function for the generator. It is equivalent
        // to 2^512 calls to next(); it can be used to generate 2^512
        // non-overlapping subsequences for parallel computations.
        void discard(uint64_t)
        {
            uint64_t t[16]{};
            for (auto i : kJump)
            {
                for(int b = 0; b < 64; ++b)
                {
                    if (i & 1ULL << b)
                    {
                        for(int j = 0; j < 16; ++j)
                            t[j] ^= s_[(j + p_) & 15];
                    }
                    (*this)();
                }
            }

            for(int j = 0; j < 16; ++j)
                s_[(j + p_) & 15] = t[j];
        }

        void seed(uint64_t seed = 0)
        {
            if (seed != 0)
            {
                for (auto&& s : s_)
                {
                    // This is a fixed-increment version of Java 8's
                    // SplittableRandom generator.
                    //
                    // See http://dx.doi.org/10.1145/2714064.2660195 and
                    // http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html
                    //
                    // It is a very fast generator passing BigCrush, and it can
                    // be useful if for some reason you absolutely want 64 bits
                    // of state; otherwise, we rather suggest to use a
                    // xorshift128+ (for moderately parallel computations) or
                    // xorshift1024* (for massively parallel computations)
                    // generator.
                    uint64_t z = (seed += UINT64_C(0x9E3779B97F4A7C15));
                    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
                    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
                    s = z ^ (z >> 31);
                }
            }
            else
            {
                std::random_device rand_dev;
                std::uniform_int_distribution<uint64_t> dist;
                for (auto&& s : s_)
                    s = dist(rand_dev);
            }
        }

        auto operator()() -> uint64_t
        {
            const uint64_t s0 = s_[p_];
            uint64_t s1 = s_[p_ = (p_ + 1) & 15];
            s1 ^= s1 << 31;                              // a
            s_[p_] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);  // b,c
            return s_[p_] * UINT64_C(1181783497276652981);
        }

    private:
        static constexpr uint64_t kJump[]{
            0x84242f96eca9c41d, 0xa3c65b8776f96855, 0x5b34a39f070b5837,
            0x4489affce4f31a1e, 0x2ffeeb0a48316f40, 0xdc2d9891fe68c022,
            0x3659132bb12fea70, 0xaac17d8efa43cab8, 0xc4cb815590989b13,
            0x5ee975283d71c93b, 0x691548c86c1bd540, 0x7910c41d10a1e6a5,
            0x0b5fc64563b3e2a8, 0x047f7684e9fc949d, 0xb99181f2d8f685ca,
            0x284600e3f30e38c3};

        uint64_t s_[16];
        int p_ = 0;
    };
}

namespace rainbow
{
    /// <summary>User-friendly wrapper for xorshift1024* generator.</summary>
    struct Random
    {
        std::uniform_real_distribution<double> dis;
        xorshift::xorshift1024star gen;

        void seed(uint64_t seed = 0) { gen.seed(seed); }

        auto operator()() { return dis(gen); }

        template <typename N>
        auto operator()(N n) -> N
        {
            return (*this)() * n;
        }

        template <typename N>
        auto operator()(N m, N n) -> N
        {
            R_ASSERT(m < n, "Parameters must be in ascending order");
            return static_cast<N>((*this)() * (n - m) + m);
        }
    };

    extern Random random;
}

#endif
