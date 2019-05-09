// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <Script/Timer.h>
#include <gtest/gtest.h>

using rainbow::TimerManager;

TEST(TimerTest, ZeroIntervalIsInvalid)
{
    TimerManager timer_manager;

    auto timer = timer_manager.set_timer([] {}, 0, 100);

    ASSERT_EQ(timer->interval(), 0);
    ASSERT_FALSE(timer->is_active());
}

TEST(TimerTest, TriggersOnce)
{
    constexpr int interval = 16;
    constexpr int repeat_count = 0;

    TimerManager timer_manager;

    bool was_called = false;
    auto timer = timer_manager.set_timer(
        [&was_called]() mutable { was_called = true; }, interval, repeat_count);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_FALSE(was_called);

    timer_manager.update(15);

    ASSERT_EQ(timer->elapsed(), 15);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_FALSE(was_called);

    timer_manager.update(5);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_FALSE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_TRUE(was_called);

    was_called = false;
    timer_manager.update(16);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_FALSE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_FALSE(was_called);
}

TEST(TimerTest, TriggersSpecifiedTimes)
{
    constexpr int interval = 16;
    constexpr int repeat_count = 5;

    TimerManager timer_manager;

    int count = 0;
    auto timer = timer_manager.set_timer(
        [&count]() mutable { ++count; }, interval, repeat_count);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_EQ(count, 0);

    timer_manager.update(17);

    ASSERT_EQ(timer->elapsed(), 1);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count - count);
    ASSERT_EQ(count, 1);

    timer_manager.update(5);

    ASSERT_EQ(timer->elapsed(), 6);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count - count);
    ASSERT_EQ(count, 1);

    timer_manager.update(10);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count - count);
    ASSERT_EQ(count, 2);

    timer_manager.update(interval * repeat_count * 2);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_FALSE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), 0);
    ASSERT_EQ(count, repeat_count + 1);
}

TEST(TimerTest, TriggersForever)
{
    constexpr int interval = 1;
    constexpr int repeat_count = -1;

    TimerManager timer_manager;

    int count = 0;
    auto timer = timer_manager.set_timer(
        [&count]() mutable { ++count; }, interval, repeat_count);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), interval);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_EQ(count, 0);

    timer_manager.update(42);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(timer->repeat_count(), repeat_count);
    ASSERT_EQ(count, 42);
}

TEST(TimerTest, TimersCanBePaused)
{
    TimerManager timer_manager;

    bool was_called = false;
    auto timer = timer_manager.set_timer(
        [&was_called]() mutable { was_called = true; }, 16, 0);
    timer_manager.update(7);

    ASSERT_EQ(timer->elapsed(), 7);
    ASSERT_TRUE(timer->is_active());
    ASSERT_FALSE(was_called);

    timer->pause();
    timer_manager.update(100);

    ASSERT_EQ(timer->elapsed(), 7);
    ASSERT_FALSE(timer->is_active());
    ASSERT_FALSE(was_called);

    timer->resume();
    timer_manager.update(100);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_FALSE(timer->is_active());
    ASSERT_TRUE(was_called);
}

TEST(TimerTest, TimersCanBeResumedAfterCompletion)
{
    TimerManager timer_manager;

    int count = 0;
    auto timer =
        timer_manager.set_timer([&count]() mutable { ++count; }, 16, 0);
    timer_manager.update(16);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_FALSE(timer->is_active());
    ASSERT_EQ(count, 1);

    timer->resume();
    timer_manager.update(15);

    ASSERT_EQ(timer->elapsed(), 15);
    ASSERT_TRUE(timer->is_active());
    ASSERT_EQ(count, 1);

    timer->resume();
    timer_manager.update(1);

    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_FALSE(timer->is_active());
    ASSERT_EQ(count, 2);
}

TEST(TimerTest, TimersAreRecycled)
{
    TimerManager timer_manager;

    auto timer = timer_manager.set_timer([] {}, 100, 100);
    timer_manager.update(16);

    ASSERT_EQ(timer->elapsed(), 16);
    ASSERT_EQ(timer->interval(), 100);
    ASSERT_EQ(timer->repeat_count(), 100);

    timer_manager.clear_timer(timer);
    auto timer2 = timer_manager.set_timer([] {}, 200, 200);

    ASSERT_EQ(timer2, timer);
    ASSERT_EQ(timer->elapsed(), 0);
    ASSERT_EQ(timer->interval(), 200);
    ASSERT_EQ(timer->repeat_count(), 200);
}

TEST(TimerTest, TimersCanBeCleared)
{
    TimerManager timer_manager;

    int count = 0;
    auto timer = timer_manager.set_timer([&count] { ++count; }, 16, -1);
    auto timer2 = timer_manager.set_timer([&count] { ++count; }, 16, -1);
    timer_manager.update(16);

    ASSERT_EQ(count, 2);

    timer_manager.clear();
    timer_manager.update(96);

    ASSERT_EQ(count, 2);

    NOT_USED(timer);
    NOT_USED(timer2);
}
