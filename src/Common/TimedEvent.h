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
	bool stopped;              ///< Whether time is accumulating.
	unsigned int accumulated;  ///< Accumulated, monotonic time.
	unsigned int timeout;      ///< Time till a tick.
};

template<typename T>
bool TimedEvent<T>::is_stopped() const
{
	return this->stopped;
}

template<typename T>
void TimedEvent<T>::set_timeout(const unsigned int timeout)
{
	this->timeout = timeout;
	this->accumulated = 0;
}

template<typename T>
void TimedEvent<T>::start()
{
	R_ASSERT(this->timeout > 0, "No time out set");
	this->stopped = false;
}

template<typename T>
void TimedEvent<T>::stop()
{
	this->stopped = true;
}

template<typename T>
void TimedEvent<T>::update(const unsigned long dt)
{
	if (this->stopped)
		return;

	this->accumulated += dt;
	while (this->accumulated >= this->timeout)
	{
		static_cast<T*>(this)->tick();
		this->accumulated -= this->timeout;
	}
}

template<typename T>
TimedEvent<T>::TimedEvent(const unsigned int timeout)
    : stopped(false), accumulated(0), timeout(timeout) { }

#endif
