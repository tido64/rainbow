// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Memory/SmallBuffer.h"

TEST(SmallBufferTest, IsReused)
{
    constexpr uint32_t kRandomCount = 42;

    const auto small_buffer = rainbow::get_small_buffer(kRandomCount);

    ASSERT_EQ(small_buffer, rainbow::get_small_buffer<int16_t>(kRandomCount));
    ASSERT_EQ(small_buffer, rainbow::get_small_buffer<int32_t>(kRandomCount));
    ASSERT_EQ(small_buffer, rainbow::get_small_buffer<int64_t>(kRandomCount));
}

TEST(SmallBufferTest, ReturnsNullOnLargeRequests)
{
    constexpr size_t kLargeishCount = 1 << 19;
    ASSERT_EQ(nullptr, rainbow::get_small_buffer(kLargeishCount));
}
