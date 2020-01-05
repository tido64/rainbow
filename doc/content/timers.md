---
id: timers
title: Timers (C++ only)
---

Timers perform delayed actions or repeat actions at set intervals.

## Timer Management

Timers are managed by a global `TimerManager`. Any void returning callable can
be used to create a timer. Timers may safely be created and cleared by timer
callbacks. Additionally, timers may clear themselves on callback without causing
error.

To access the timer manager, use the static `Get()` method:

```c++
TimerManager* timer_manager = TimerManager::Get();
```

### Creating and Clearing Timers

```c++
auto TimerManager::set_timer(Timer::Closure,
                             const int interval,
                             const int repeat_count) -> Timer*;
auto TimerManager::clear_timer(Timer*) -> void;
```

`Timer::Closure` is defined as a callable whose signature is `void()`. The
`interval` is the time, in milliseconds, to wait before the first call and
between any subsequent calls. A zero interval will immediately set the timer in
a finished state. Calls are _repeated_ `repeat_count` times, e.g. a repeat count
of four makes the timer perform an action five times. For infinity, use any
negative value. `TimerManager::set_timer()` returns a `Timer` object which can
be used to pause and resume the timer, as well as querying its state.

A `Timer` that has run its course, will be put in a paused state. It can be
resumed but the repeat count will not be restored.

### Pausing and Resuming Timers

```c++
void Timer::pause();
void Timer::resume();
```

Pausing a timer will freeze it in its current state until it is resumed.

### Timer Information

```c++
auto Timer::is_active() const -> bool;
```

Returns whether the timer is currently active. This method returns `true` while
the timer is running normally, and `false` otherwise.

```c++
auto Timer::elapsed() const -> int;
```

Returns the elapsed time.

```c++
auto Timer::interval() const -> int;
```

Returns the timer's delay/interval in milliseconds.

```c++
auto Timer::repeat_count() const -> int;
```

Returns the number of times the action will be repeated. Note that this number
excludes the first call.

## Example

The following example creates four timers. Two that are performed 5 times
(repeated 4 times) with an interval of 500 ms, and two that are performed after
a 500 ms delay. We use a freestanding function, a functor, a lambda function,
and a bound member function to create them. For simplicity's sake, we only clear
the first repeated timer.

```c++
#include <functional>

#include "Script/GameBase.h"
#include "Script/Timer.h"

void action()
{
  LOGI("A freestanding function was called.");
}

class RepeatableAction
{
public:
  void action()
  {
    LOGI("A bound, repeated action was performed: %i time(s)", ++counter_);
  }

  void operator()()
  {
    LOGI("A repeated action was performed: %i time(s)", ++counter_);
  }

private:
  int counter_ = 0;
};

class TimerExample final : public rainbow::GameBase
{
public:
  TimerExample(rainbow::Director& director)
    : rainbow::GameBase(director), repeated_(nullptr)
  {
  }

private:
  rainbow::Timer* repeated_;
  RepeatableAction action_;

  void init_impl(const rainbow::Vec2i&) override
  {
    auto timer_manager = rainbow::TimerManager::Get();

    // Create a delayed action using a function.
    timer_manager->set_timer(&action, 500, 0);

    // Create a repeated action using a functor.
    repeated_ = timer_manager->set_timer(RepeatableAction{}, 500, 4);

    // Create a delayed action using a lambda function.
    timer_manager->set_timer(
      [] { LOGI("A delayed action was performed."); }, 500, 0);

    // Create a repeated action using a bound member function.
    timer_manager->set_timer(
      std::bind(&RepeatableAction::action, action_), 500, 4);
  }

  void update_impl(uint64_t) override
  {
    if (repeated_ && !repeated_->is_active())
    {
      rainbow::TimerManager::Get()->clear_timer(repeated_);
      repeated_ = nullptr;
      LOGI("The repeated timer was cleared.");
    }
  }
};

auto rainbow::GameBase::create(rainbow::Director& director)
  -> std::unique_ptr<rainbow::GameBase>
{
  return std::make_unique<TimerExample>(director);
}
```

Output:

```console
[1428273728390|INFO] A freestanding function was called.
[1428273728390|INFO] A repeated action was performed: 1 time(s)
[1428273728390|INFO] A delayed action was performed.
[1428273728390|INFO] A bound, repeated action was performed: 1 time(s)
[1428273728847|INFO] A repeated action was performed: 2 time(s)
[1428273728847|INFO] A bound, repeated action was performed: 2 time(s)
[1428273729352|INFO] A repeated action was performed: 3 time(s)
[1428273729352|INFO] A bound, repeated action was performed: 3 time(s)
[1428273729863|INFO] A repeated action was performed: 4 time(s)
[1428273729863|INFO] A bound, repeated action was performed: 4 time(s)
[1428273730380|INFO] A repeated action was performed: 5 time(s)
[1428273730380|INFO] A bound, repeated action was performed: 5 time(s)
[1428273730380|INFO] The repeated timer was cleared.
```

## Caveats and Known Limitations

Timer handlers are reused. This implies that an old handler may be used to
manipulate a more recent timer.
