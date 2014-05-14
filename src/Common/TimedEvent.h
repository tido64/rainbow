// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_TIMEDEVENT_H_
#define COMMON_TIMEDEVENT_H_

#include "Common/Debug.h"

/// A ticker with a configurable time interval.
template<typename T>
class TimedEvent
{
public:
	/// Returns whether this ticker is running.
	bool is_stopped() const;

	/// Sets time till a tick.
	void set_timeout(const unsigned int timeout);

	/// Starts accumulating time.
	void start();

	/// Stops accumulating time.
	void stop();

	/// Accumulates time and calls \c tick() when time is up.
	void update(const unsigned long dt);

protected:
	TimedEvent(const unsigned int timeout);
	~TimedEvent() = default;

private:
	unsigned int accumulated_;  ///< Accumulated, monotonic time.
	unsigned int timeout_;      ///< Time till a tick.
	bool stopped_;              ///< Whether time is being accumulated.
};

template<typename T>
bool TimedEvent<T>::is_stopped() const
{
	return stopped_;
}

template<typename T>
void TimedEvent<T>::set_timeout(const unsigned int timeout)
{
	timeout_ = timeout;
	accumulated_ = 0;
}

template<typename T>
void TimedEvent<T>::start()
{
	R_ASSERT(timeout_ > 0, "No time out set");
	stopped_ = false;
}

template<typename T>
void TimedEvent<T>::stop()
{
	stopped_ = true;
}

template<typename T>
void TimedEvent<T>::update(const unsigned long dt)
{
	if (stopped_)
		return;

	accumulated_ += dt;
	while (accumulated_ >= timeout_)
	{
		static_cast<T*>(this)->tick();
		accumulated_ -= timeout_;
	}
}

template<typename T>
TimedEvent<T>::TimedEvent(const unsigned int timeout)
    : accumulated_(0), timeout_(timeout), stopped_(false) { }

#endif
