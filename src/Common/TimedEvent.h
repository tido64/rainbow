#ifndef COMMON_TIMEDEVENT_H_
#define COMMON_TIMEDEVENT_H_

#include "Common/Debug.h"

/// A ticker with a configurable time interval.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class TimedEvent
{
public:
	/// Returns whether this ticker is running.
	inline bool is_stopped();

	/// Sets time till a tick.
	inline void set_timeout(const unsigned int timeout);

	/// Starts accumulating time.
	inline void start();

	/// Stops accumulating time.
	inline void stop();

	/// Accumulates time and calls \c tick() when time is up.
	void update(const unsigned long dt);

protected:
	inline TimedEvent(const unsigned int timeout);

private:
	bool stopped;              ///< Whether time is accumulating.
	unsigned int accumulated;  ///< Accumulated, monotonic time.
	unsigned int timeout;      ///< Time till a tick.
};

template<class T>
bool TimedEvent<T>::is_stopped()
{
	return this->stopped;
}

template<class T>
void TimedEvent<T>::set_timeout(const unsigned int timeout)
{
	this->timeout = timeout;
	this->accumulated = 0;
}

template<class T>
void TimedEvent<T>::start()
{
	R_ASSERT(this->timeout > 0, "No time out set");
	this->stopped = false;
}

template<class T>
void TimedEvent<T>::stop()
{
	this->stopped = true;
}

template<class T>
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

template<class T>
TimedEvent<T>::TimedEvent(const unsigned int timeout) :
	stopped(false), accumulated(0), timeout(timeout) { }

#endif
