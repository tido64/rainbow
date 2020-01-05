// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/TypeCast.h"

#include <gtest/gtest.h>

TEST(TypeCastTest, Truncates)
{
    ASSERT_EQ(rainbow::truncate<int>(1.0F), 1);
    ASSERT_EQ(rainbow::truncate<int>(1.1F), 1);
    ASSERT_EQ(rainbow::truncate<int>(1.9F), 1);
    ASSERT_EQ(rainbow::truncate<int>(-1.9F), -1);
    ASSERT_EQ(rainbow::truncate<int>(-1.1F), -1);
    ASSERT_EQ(rainbow::truncate<int>(-1.0F), -1);
}
