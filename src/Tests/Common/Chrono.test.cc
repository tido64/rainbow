// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Chrono.h"

#include <gtest/gtest.h>

using rainbow::Chrono;

TEST(ChronoTest, ReturnsCurrentTimeInMilliSeconds)
{
    const auto now = std::chrono::system_clock::now();
    ASSERT_EQ(Chrono::system_now(),
              std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()));
}

TEST(ChronoTest, ReturnsEpochTimeInSeconds)
{
    const auto now = std::chrono::steady_clock::now();
    ASSERT_EQ(Chrono::time_since_epoch(),
              std::chrono::duration_cast<std::chrono::seconds>(
                  now.time_since_epoch()));
}

TEST(ChronoTest, TimeDifferenceBetweenUpdates)
{
    Chrono chrono;
    ASSERT_EQ(chrono.delta(), 0);

    for (auto i = 0; i < 10; ++i)
    {
        Chrono::sleep(int64_t(2) << i);
        chrono.tick();
        ASSERT_GE(chrono.delta(), i);
    }
}
