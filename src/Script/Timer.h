// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TIMER_H_
#define SCRIPT_TIMER_H_

#include <algorithm>
#include <deque>
#include <functional>

#include "Common/Global.h"

class Timer : private NonCopyable<Timer>
{
public:
	using Closure = std::function<void()>;

	Timer(Closure func,
	      const int interval,
	      const int repeat_count,
	      const int id = 0)
	    : active_(true), interval_(interval), elapsed_(0),
	      countdown_(repeat_count), tick_(std::move(func)),
	      repeat_count_(repeat_count), free_(-1), id_(id) {}

	bool is_active() const { return active_ && interval_ > 0; }
	int elapsed() const { return elapsed_; }
	int interval() const { return interval_; }
	int repeat_count() const { return repeat_count_; }

	void pause() { active_ = false; }
	void resume() { active_ = true; }

private:
	bool active_;
	int interval_;
	int elapsed_;
	int countdown_;
	Closure tick_;
	int repeat_count_;
	int free_;
	const int id_;

	int clear(const int free);
	void update(const unsigned long dt);

	Timer& operator=(Timer&& t);

	friend class TimerManager;
};

class TimerManager : public Global<TimerManager>
{
public:
	TimerManager() : free_(-1) { make_global(); }

	void clear_timer(Timer *t);

	Timer* set_timer(Timer::Closure func,
	                 const int interval,
	                 const int repeat_count);

	void update(const unsigned long dt);

private:
	std::deque<Timer> timers_;
	int free_;
};

#endif
