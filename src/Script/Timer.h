// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TIMER_H_
#define SCRIPT_TIMER_H_

#include <functional>

#include "Common/Global.h"
#include "Common/Passkey.h"
#include "Memory/Pool.h"

namespace rainbow
{
    class TimerManager;

    class Timer : private NonCopyable<Timer>
    {
    public:
        auto elapsed() const { return elapsed_; }
        auto interval() const { return interval_; }
        auto is_active() const { return active_ && interval_ > 0; }
        auto repeat_count() const { return repeat_count_; }

        void pause() { active_ = false; }
        void resume() { active_ = true; }

        // Internal API

        template <typename F>
        Timer(F&& func,
              int interval,
              int repeat_count,
              const Passkey<TimerManager>&)
            : active_(true), interval_(interval), elapsed_(0),
              repeat_count_(repeat_count), tick_(std::forward<F>(func))
        {
        }

        void dispose(const Passkey<TimerManager>&);

        template <typename F>
        void reset(F&& func,
                   int interval,
                   int repeat_count,
                   const Passkey<TimerManager>&)
        {
            active_ = true;
            interval_ = interval;
            elapsed_ = 0;
            repeat_count_ = repeat_count;
            tick_ = std::forward<F>(func);
            repeat_count_ = repeat_count;
        }

        void update(uint64_t dt, const Passkey<TimerManager>&);

    private:
        bool active_;
        int interval_;
        int elapsed_;
        int repeat_count_;
        std::function<void()> tick_;
    };

    class TimerManager : public Global<TimerManager>
    {
    public:
        TimerManager() { make_global(); }

        void clear() { timers_.clear(); }

        void clear_timer(Timer* t)
        {
            timers_.release(t, Passkey<TimerManager>{});
        }

        template <typename F>
        auto set_timer(F&& func, int interval, int repeat_count) -> Timer*
        {
            return timers_.construct(std::forward<F>(func),
                                     interval,
                                     repeat_count,
                                     Passkey<TimerManager>{});
        }

        void update(uint64_t dt);

    private:
        Pool<Timer> timers_;
    };
}  // namespace rainbow

#endif
