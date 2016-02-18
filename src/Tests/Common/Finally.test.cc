// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Finally.h"

TEST(FinallyTest, ExecutesOnScopeExit)
{
    bool did_execute_final_act = false;
    {
        auto _ = rainbow::finally([&did_execute_final_act]() mutable {
            did_execute_final_act = true;
        });
        ASSERT_FALSE(did_execute_final_act);
    }
    ASSERT_TRUE(did_execute_final_act);
}

TEST(FinallyTest, IsCancellable)
{
    bool did_execute_final_act = false;
    {
        auto final_act = rainbow::finally([&did_execute_final_act]() mutable {
            did_execute_final_act = true;
        });
        ASSERT_FALSE(did_execute_final_act);
        final_act.cancel();
    }
    ASSERT_FALSE(did_execute_final_act);
}

TEST(FinallyTest, IsReplaceable)
{
    int final_act_id = 0;
    {
        auto final_act = rainbow::finally<std::function<void()>>(
            [&final_act_id]() mutable { final_act_id = 1; });
        final_act = rainbow::finally<std::function<void()>>(
            [&final_act_id]() mutable { final_act_id = 2; });
        ASSERT_EQ(0, final_act_id);
    }
    ASSERT_EQ(2, final_act_id);
}
