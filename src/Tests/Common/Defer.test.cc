// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Defer.h"

#include <gtest/gtest.h>

TEST(DeferTest, CallsClosure)
{
    constexpr int iterations = 5;

    int count = 0;
    for (int i = 0; i < iterations; ++i)
    {
        auto token = rainbow::defer([&count]() mutable { ++count; });
        ASSERT_EQ(count, i);
    }

    ASSERT_EQ(count, iterations);
}
