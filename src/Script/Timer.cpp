// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Timer.h"

using rainbow::Passkey;

auto Timer::clear(int free, const Passkey<TimerManager>&) -> int
{
    interval_ = 0;
    tick_ = {};  // Always clear as resources may be retained in the closure.
    free_ = free;
    return id_;
}

void Timer::reset(Closure func,
                  int interval,
                  int repeat_count,
                  const Passkey<TimerManager>&)
{
    active_ = true;
    elapsed_ = 0;
    interval_ = interval;
    countdown_ = repeat_count;
    repeat_count_ = repeat_count;
    tick_ = std::move(func);
    free_ = -1;
}

void Timer::update(uint64_t dt, const Passkey<TimerManager>&)
{
    if (!is_active())
        return;

    elapsed_ += dt;
    const int ticks = elapsed_ / interval_;
    for (int i = 0; i < ticks; ++i)
    {
        tick_();
        if (countdown_ == 0)
        {
            interval_ = 0;
            return;
        }
        else if (countdown_ > 0)
            --countdown_;
    }
    elapsed_ -= ticks * interval_;
}

void TimerManager::clear_timer(Timer* t)
{
    free_ = t->clear(free_, {});
}

auto TimerManager::set_timer(Timer::Closure func,
                             int interval,
                             int repeat_count) -> Timer*
{
    Timer* t;
    if (free_ < 0)
    {
        timers_.emplace_back(std::move(func),
                             interval,
                             repeat_count,
                             static_cast<int>(timers_.size()));
        t = &timers_.back();
    }
    else
    {
        t = &timers_[free_];
        free_ = t->next_free();
        t->reset(std::move(func), interval, repeat_count, {});
    }
    return t;
}

void TimerManager::update(uint64_t dt)
{
    const size_t count = timers_.size();
    for (size_t i = 0; i < count; ++i)
        timers_[i].update(dt, {});
}
