// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Logging.h"
#include "Common/UTF8.h"

namespace
{
    constexpr unsigned char kInvalidUTF8[]{
        'B', 'l',  'o', 'w',  'z', 'y', ' ', 'n', 'i', 'g', 'h',
        't', '-',  'f', 'r',  'u', 'm', 'p', 's', ' ', 'v', 'e',
        'x', '\'', 'd', 0xff, 'J', 'a', 'c', 'k', ' ', 'Q', '.'};

    constexpr unsigned char kUTF8[]{
        0xc3, 0x85,              // LATIN CAPITAL LETTER A WITH RING ABOVE
        0xc3, 0x86,              // LATIN CAPITAL LETTER AE
        0xc3, 0x98,              // LATIN CAPITAL LETTER O WITH STROKE
        0xc3, 0xa5,              // LATIN SMALL LETTER A WITH RING ABOVE
        0xc3, 0xa6,              // LATIN SMALL LETTER AE
        0xc3, 0xb8,              // LATIN SMALL LETTER O WITH STROKE
        0xe2, 0x82, 0xac,        // EURO SIGN
        0xf0, 0x9f, 0x92, 0xa9,  // PILE OF POO
        0x00};

    constexpr uint32_t kUTF32[]{
        0x00c5, 0x00c6, 0x00d8, 0x00e5, 0x00e6, 0x00f8, 0x20ac, 0x1f4a9};
}

TEST(AlgorithmTest, ClampsValues)
{
    ASSERT_EQ(1, rainbow::clamp(10, 0, 1));
    ASSERT_EQ(0, rainbow::clamp(-10, 0, 1));
    ASSERT_EQ(0, rainbow::clamp(0, 0, 1));
    ASSERT_EQ(1, rainbow::clamp(1, 0, 1));
}

TEST(AlgorithmTest, ConvertsRadiansToDegrees)
{
    ASSERT_TRUE(rainbow::is_equal(static_cast<float>(kPi),
                                  rainbow::radians(rainbow::degrees(kPi))));
}

TEST(AlgorithmTest, ApproximatesInverseSquareRoot)
{
    const double kErrorMargin = 0.017478;
    for (double f = 0.01; f < 10000; f += 0.01)
        ASSERT_NEAR(1 / sqrt(f), rainbow::fast_invsqrt(f), kErrorMargin);
}

TEST(AlgorithmTest, ApproximatesFloatEquality)
{
    ASSERT_TRUE(rainbow::is_equal(0.0f, 0.0f));
    ASSERT_FALSE(rainbow::is_equal(0.0f, 0.00001f));
    ASSERT_FALSE(rainbow::is_equal(0.0f, -0.00001f));
    ASSERT_TRUE(rainbow::is_equal(3.14285714f, 22.0f / 7.0f));
}

TEST(AlgorithmTest, IsPowerOfTwo)
{
    unsigned int p = 1;
    for (unsigned int i = 0; i < 100; ++i)
    {
        if (i == p)
        {
            ASSERT_TRUE(rainbow::is_pow2(i));
            p *= 2;
            continue;
        }
        ASSERT_FALSE(rainbow::is_pow2(i));
    }
}

TEST(AlgorithmTest, ReturnsNextPowerOfTwo)
{
    unsigned int p = 1;
    for (unsigned int i = 1; i < 100; ++i)
    {
        ASSERT_EQ(p, rainbow::next_pow2(i));
        if (i == p)
            p *= 2;
    }
}

TEST(AlgorithmTest, QuickErasesElementsInContainer)
{
    std::vector<int> v{1, 2, 3, 4, 5};
    rainbow::quick_erase(v, v.begin());

    ASSERT_EQ(4u, v.size());
    ASSERT_EQ(5, v[0]);
    ASSERT_EQ(2, v[1]);
    ASSERT_EQ(3, v[2]);
    ASSERT_EQ(4, v[3]);

    rainbow::quick_erase(v, v.begin() + 1);

    ASSERT_EQ(3u, v.size());
    ASSERT_EQ(5, v[0]);
    ASSERT_EQ(4, v[1]);
    ASSERT_EQ(3, v[2]);

    rainbow::quick_erase(v, v.end() - 1);

    ASSERT_EQ(2u, v.size());
    ASSERT_EQ(5, v[0]);
    ASSERT_EQ(4, v[1]);
}

TEST(AlgorithmTest, ConvertsDegreesToRadians)
{
    ASSERT_TRUE(rainbow::is_equal(static_cast<float>(kPi),
                                  rainbow::degrees(rainbow::radians(kPi))));
}

TEST(AlgorithmTest, RemovesValuesFromContainer)
{
    std::vector<int> v{1, 2, 3, 4, 3};
    rainbow::remove(v, 3);

    ASSERT_EQ(4u, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(2, v[1]);
    ASSERT_EQ(4, v[2]);
    ASSERT_EQ(3, v[3]);

    rainbow::remove(v, 3);

    ASSERT_EQ(3u, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(2, v[1]);
    ASSERT_EQ(4, v[2]);

    rainbow::remove(v, 9000);

    ASSERT_EQ(3u, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(2, v[1]);
    ASSERT_EQ(4, v[2]);
}

TEST(AlgorithmTest, RemovesValuesSatisfyingPredicatesFromContainer)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    rainbow::remove_if(v, [](int i) { return i % 2 == 0; });

    ASSERT_EQ(3u, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(3, v[1]);
    ASSERT_EQ(5, v[2]);

    rainbow::remove_if(v, [](int i) { return i > 5; });

    ASSERT_EQ(3u, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(3, v[1]);
    ASSERT_EQ(5, v[2]);
}

TEST(AlgorithmTest, RotatesLeft)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    rainbow::rotate_left(nums);

    ASSERT_EQ(5u, nums.size());
    ASSERT_EQ(2, nums[0]);
    ASSERT_EQ(3, nums[1]);
    ASSERT_EQ(4, nums[2]);
    ASSERT_EQ(5, nums[3]);
    ASSERT_EQ(1, nums[4]);

    rainbow::rotate_left(nums);

    ASSERT_EQ(5u, nums.size());
    ASSERT_EQ(3, nums[0]);
    ASSERT_EQ(4, nums[1]);
    ASSERT_EQ(5, nums[2]);
    ASSERT_EQ(1, nums[3]);
    ASSERT_EQ(2, nums[4]);
}

TEST(AlgorithmTest, RotatesRight)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    rainbow::rotate_right(nums);

    ASSERT_EQ(5u, nums.size());
    ASSERT_EQ(5, nums[0]);
    ASSERT_EQ(1, nums[1]);
    ASSERT_EQ(2, nums[2]);
    ASSERT_EQ(3, nums[3]);
    ASSERT_EQ(4, nums[4]);

    rainbow::rotate_right(nums);

    ASSERT_EQ(5u, nums.size());
    ASSERT_EQ(4, nums[0]);
    ASSERT_EQ(5, nums[1]);
    ASSERT_EQ(1, nums[2]);
    ASSERT_EQ(2, nums[3]);
    ASSERT_EQ(3, nums[4]);
}

TEST(AlgorithmTest, ExtractsSignOfRealNumbers)
{
    ASSERT_GT(0, rainbow::signum(-10));
    ASSERT_EQ(0, rainbow::signum(0));
    ASSERT_LT(0, rainbow::signum(10));
}

TEST(AlgorithmTest, IteratesUTF8String)
{
    size_t i = 0;
    rainbow::for_each_utf8(reinterpret_cast<const char*>(kUTF8),
                           [&i](uint32_t ch) {
                               ASSERT_EQ(kUTF32[i], ch);
                               ++i;
                           });
    ASSERT_EQ(rainbow::array_size(kUTF32), i);

    i = 0;
    rainbow::for_each_utf8(reinterpret_cast<const char*>(kInvalidUTF8),
                           [&i](uint32_t ch) {
                               ASSERT_EQ(kInvalidUTF8[i], ch);
                               ++i;
                           });
    ASSERT_EQ(25u, i);
}
