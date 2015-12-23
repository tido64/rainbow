// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Timer.h"

void TimerManager::clear_timer(Timer* t) { free_ = t->clear(free_); }

Timer* TimerManager::set_timer(Timer::Closure func,
                               int interval,
                               int repeat_count)
{
	Timer* t;
	if (free_ < 0)
	{
		timers_.emplace_back(
		    std::move(func), interval, repeat_count, timers_.size());
		t = &timers_.back();
	}
	else
	{
		t = &timers_[free_];
		free_ = t->free_;
		*t = Timer(std::move(func), interval, repeat_count);
	}
	return t;
}

void TimerManager::update(unsigned long dt)
{
	const size_t count = timers_.size();
	for (size_t i = 0; i < count; ++i)
		timers_[i].update(dt);
}

int Timer::clear(int free)
{
	interval_ = 0;
	tick_ = {};  // Always clear as resources may be retained in the closure.
	free_ = free;
	return id_;
}

void Timer::update(unsigned long dt)
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

Timer& Timer::operator=(Timer&& t)
{
	active_ = true;
	elapsed_ = 0;
	interval_ = t.interval_;
	countdown_ = t.repeat_count_;
	repeat_count_ = t.repeat_count_;
	tick_ = std::move(t.tick_);
	free_ = -1;
	return *this;
}
