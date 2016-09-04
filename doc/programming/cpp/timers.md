# Timers

Timers perform delayed actions or repeat actions at set intervals.

## Timer Management

Timers are managed by a global `TimerManager`. Any void returning callable
can be used to create a timer. Timers may safely be created and cleared by timer
callbacks. Additionally, timers may clear themselves on callback without causing
error.

To access the timer manager, use the static `Get()` method:

```c++
TimerManager *timer_manager = TimerManager::Get();
```

### Creating and Clearing Timers

```c++
Timer*  TimerManager::set_timer    (Timer::Closure func,
                                    const int interval,
                                    const int repeat_count);
  void  TimerManager::clear_timer  (Timer *t);
```

`Timer::Closure` is defined as a callable whose signature is `void()`. The
`interval` is the time, in milliseconds, to wait before the first call and
between any subsequent calls. A 0 interval will immediately set the timer in a
finished state. Calls are _repeated_ `repeat_count` times, e.g. a repeat count
of 4 makes the timer perform an action 5 times. For infinity, use any negative
value. `TimerManager::set_timer()` returns a `Timer` object which can be used to
pause and resume the timer, as well as querying information.

### Pausing and Resuming Timers

```c++
void  Timer::pause   ();
void  Timer::resume  ();
```

Pausing a timer will freeze it in its current state until it is resumed.

### Timer Information

```c++
bool  Timer::is_active  () const;
```

Returns whether the timer is currently active. This method returns `true` while
the timer is running normally, and `false` when it is finished or has been
paused.

```c++
int  Timer::elapsed  () const;
```

Returns the elapsed time.

```c++
int  Timer::interval  () const;
```

Returns the timer's delay/interval in milliseconds.

```c++
int  Timer::repeat_count  () const;
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

class TimerExample final : public GameBase
{
public:
    TimerExample(rainbow::Director &director)
        : GameBase(director), repeated_(nullptr)  {}

    void init(const Vec2i &) override
    {
        // Create a delayed action using a function.
        TimerManager::Get()->set_timer(&action, 500, 0);

        // Create a repeated action using a functor.
        repeated_ = TimerManager::Get()->set_timer(RepeatableAction(), 500, 4);

        // Create a delayed action using a lambda function.
        TimerManager::Get()->set_timer(
            [] { LOGI("A delayed action was performed."); }, 500, 0);

        // Create a repeated action using a bound member function.
        TimerManager::Get()->set_timer(
            std::bind(&RepeatableAction::action, action_), 500, 4);
    }

    void update(const unsigned long) override
    {
        if (repeated_ && !repeated_->is_active())
        {
            TimerManager::Get()->clear_timer(repeated_);
            repeated_ = nullptr;
            LOGI("The repeated timer was cleared.");
        }
    }

private:
    Timer *repeated_;
    RepeatableAction action_;
};

GameBase* GameBase::create(rainbow::Director &director)
{
    return new TimerExample(director);
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

## Caveats and Known Issues

Timer handlers are reused. This implies that an old handler may be used to
manipulate a more recent timer.
