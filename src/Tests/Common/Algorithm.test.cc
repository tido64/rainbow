// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Algorithm.h"

#include <functional>

#include <gtest/gtest.h>

#include "Common/Logging.h"

namespace
{
    constexpr auto kPi = rainbow::kPi<float>;

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
}  // namespace

TEST(AlgorithmTest, ApproximatesFloatEquality)
{
    auto not_equal = std::not_fn(rainbow::are_equal<float>);

    ASSERT_PRED2(rainbow::are_equal<float>, 0.0f, 0.0f);
    ASSERT_PRED2(rainbow::are_equal<float>, 0.0f, -0.0f);
    ASSERT_PRED2(not_equal, 0.0f, 0.00001f);
    ASSERT_PRED2(not_equal, 0.0f, -0.00001f);
    ASSERT_PRED2(rainbow::are_equal<float>, 3.14285714f, 22.0f / 7.0f);
}

TEST(AlgorithmTest, RoundsUpToNearestPowerOfTwo)
{
    unsigned int p = 1;
    for (unsigned int i = 1; i < 100; ++i)
    {
        ASSERT_EQ(rainbow::ceil_pow2(i), p);
        if (i == p)
            p *= 2;
    }
}

TEST(AlgorithmTest, ConvertsRadiansToDegrees)
{
    ASSERT_PRED2(rainbow::are_equal<float>,
                 kPi,
                 rainbow::radians(rainbow::degrees(kPi)));
}

TEST(AlgorithmTest, ApproximatesInverseSquareRoot)
{
    constexpr double kErrorMargin = 0.017478;
    for (int i = 1; i < 1000000; ++i)
    {
        const double f = i / 100.0;
        ASSERT_NEAR(1 / std::sqrt(f), rainbow::fast_invsqrt(f), kErrorMargin);
    }
}

TEST(AlgorithmTest, FindsValueInContainer)
{
    auto iter = std::begin(kUTF32);
    for (auto&& value : kUTF32)
        ASSERT_EQ(rainbow::find(kUTF32, value), iter++);
    ASSERT_EQ(iter, std::end(kUTF32));
    ASSERT_EQ(rainbow::find(kUTF8, 0xc3), std::begin(kUTF8));
}

TEST(AlgorithmTest, RoundsDownToNearestPowerOfTwo)
{
    ASSERT_EQ(rainbow::floor_pow2(0), 0u);

    unsigned int p = 1;
    for (unsigned int i = 1; i < 100; ++i)
    {
        if (i == p * 2)
            p = i;
        ASSERT_EQ(rainbow::floor_pow2(i), p);
    }
}

TEST(AlgorithmTest, ApproximatesZeroFloat)
{
    auto definitely_not_zero = std::not_fn(rainbow::is_almost_zero);

    ASSERT_PRED1(rainbow::is_almost_zero, 0.0f);
    ASSERT_PRED1(rainbow::is_almost_zero, -0.0f);

    constexpr float kNotZero = std::numeric_limits<float>::epsilon() * 10.0f;

    ASSERT_PRED1(definitely_not_zero, kNotZero);
    ASSERT_PRED1(definitely_not_zero, -kNotZero);
    ASSERT_PRED1(definitely_not_zero, std::numeric_limits<float>::lowest());
    ASSERT_PRED1(definitely_not_zero, std::numeric_limits<float>::max());
}

TEST(AlgorithmTest, IsPowerOfTwo)
{
    auto not_pow2 = std::not_fn(rainbow::is_pow2);

    unsigned int p = 1;
    for (unsigned int i = 0; i < 100; ++i)
    {
        if (i == p)
        {
            ASSERT_PRED1(rainbow::is_pow2, i);
            p *= 2;
            continue;
        }
        ASSERT_PRED1(not_pow2, i);
    }
}

TEST(AlgorithmTest, MakesFourCC)
{
    ASSERT_EQ(rainbow::make_fourcc('D', 'D', 'S', ' '), 0x20534444u);
    ASSERT_EQ(rainbow::make_fourcc('P', 'V', 'R', 3), 0x03525650u);
}

TEST(AlgorithmTest, QuickErasesElementsInContainer)
{
    std::vector<int> v{1, 2, 3, 4, 5};
    rainbow::quick_erase(v, v.begin());

    ASSERT_EQ(v.size(), 4u);
    ASSERT_EQ(v[0], 5);
    ASSERT_EQ(v[1], 2);
    ASSERT_EQ(v[2], 3);
    ASSERT_EQ(v[3], 4);

    rainbow::quick_erase(v, v.begin() + 1);

    ASSERT_EQ(v.size(), 3u);
    ASSERT_EQ(v[0], 5);
    ASSERT_EQ(v[1], 4);
    ASSERT_EQ(v[2], 3);

    rainbow::quick_erase(v, v.end() - 1);

    ASSERT_EQ(v.size(), 2u);
    ASSERT_EQ(v[0], 5);
    ASSERT_EQ(v[1], 4);
}

TEST(AlgorithmTest, ConvertsDegreesToRadians)
{
    ASSERT_PRED2(rainbow::are_equal<float>,
                 kPi,
                 rainbow::degrees(rainbow::radians(kPi)));
}

TEST(AlgorithmTest, RemovesValuesFromContainer)
{
    std::vector<int> v{1, 2, 3, 4, 3};
    rainbow::remove(v, 3);

    ASSERT_EQ(v.size(), 4u);
    ASSERT_EQ(v[0], 1);
    ASSERT_EQ(v[1], 2);
    ASSERT_EQ(v[2], 4);
    ASSERT_EQ(v[3], 3);

    rainbow::remove(v, 3);

    ASSERT_EQ(v.size(), 3u);
    ASSERT_EQ(v[0], 1);
    ASSERT_EQ(v[1], 2);
    ASSERT_EQ(v[2], 4);

    rainbow::remove(v, 9000);

    ASSERT_EQ(v.size(), 3u);
    ASSERT_EQ(v[0], 1);
    ASSERT_EQ(v[1], 2);
    ASSERT_EQ(v[2], 4);
}

TEST(AlgorithmTest, RemovesValuesSatisfyingPredicatesFromContainer)
{
    std::vector<int> v{1, 2, 3, 4, 5, 6};
    rainbow::remove_if(v, [](int i) { return i % 2 == 0; });

    ASSERT_EQ(v.size(), 3u);
    ASSERT_EQ(v[0], 1);
    ASSERT_EQ(v[1], 3);
    ASSERT_EQ(v[2], 5);

    rainbow::remove_if(v, [](int i) { return i > 5; });

    ASSERT_EQ(v.size(), 3u);
    ASSERT_EQ(v[0], 1);
    ASSERT_EQ(v[1], 3);
    ASSERT_EQ(v[2], 5);
}

TEST(AlgorithmTest, RotatesLeft)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    rainbow::rotate_left(nums);

    ASSERT_EQ(nums.size(), 5u);
    ASSERT_EQ(nums[0], 2);
    ASSERT_EQ(nums[1], 3);
    ASSERT_EQ(nums[2], 4);
    ASSERT_EQ(nums[3], 5);
    ASSERT_EQ(nums[4], 1);

    rainbow::rotate_left(nums);

    ASSERT_EQ(nums.size(), 5u);
    ASSERT_EQ(nums[0], 3);
    ASSERT_EQ(nums[1], 4);
    ASSERT_EQ(nums[2], 5);
    ASSERT_EQ(nums[3], 1);
    ASSERT_EQ(nums[4], 2);
}

TEST(AlgorithmTest, RotatesRight)
{
    std::vector<int> nums{1, 2, 3, 4, 5};
    rainbow::rotate_right(nums);

    ASSERT_EQ(nums.size(), 5u);
    ASSERT_EQ(nums[0], 5);
    ASSERT_EQ(nums[1], 1);
    ASSERT_EQ(nums[2], 2);
    ASSERT_EQ(nums[3], 3);
    ASSERT_EQ(nums[4], 4);

    rainbow::rotate_right(nums);

    ASSERT_EQ(nums.size(), 5u);
    ASSERT_EQ(nums[0], 4);
    ASSERT_EQ(nums[1], 5);
    ASSERT_EQ(nums[2], 1);
    ASSERT_EQ(nums[3], 2);
    ASSERT_EQ(nums[4], 3);
}

TEST(AlgorithmTest, ExtractsSignOfRealNumbers)
{
    ASSERT_GT(0, rainbow::signum(-10));
    ASSERT_EQ(rainbow::signum(0), 0);
    ASSERT_LT(0, rainbow::signum(10));
}
