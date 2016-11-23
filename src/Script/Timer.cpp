// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Timer.h"

using rainbow::Passkey;
using rainbow::Timer;
using rainbow::TimerManager;

void Timer::dispose(const Passkey<TimerManager>&)
{
    interval_ = 0;
    tick_ = {};  // Always clear as resources may be retained in the closure.
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

        if (countdown_ > 0)
            --countdown_;
    }
    elapsed_ -= ticks * interval_;
}

void TimerManager::update(uint64_t dt)
{
    for_each(timers_, [dt](Timer& t) { t.update(dt, {}); });
}
