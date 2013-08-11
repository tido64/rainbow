#ifndef COMMON_CHRONOTRIGGER_H_
#define COMMON_CHRONOTRIGGER_H_

#include "Common/Debug.h"

/// A ticker with a configurable time interval.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class ChronoTrigger
{
public:
	inline ChronoTrigger();

	inline ChronoTrigger(const unsigned int timeout);

	virtual ~ChronoTrigger();

	/// Returns whether this ticker is running.
	inline bool is_stopped();

	/// Sets time till a tick.
	inline void set_timeout(const unsigned int timeout);

	/// Starts accumulating time.
	inline void start();

	/// Stops accumulating time.
	inline void stop();

	/// Called every tick.
	inline void tick();

	/// Accumulates time and calls \c tick() when time is up.
	void update(const unsigned long dt);

private:
	bool stopped;              ///< Whether time is accumulating.
	unsigned int accumulated;  ///< Accumulated, monotonic time.
	unsigned int trigger;      ///< Time till a tick.

	virtual void tick_impl() = 0;
};

ChronoTrigger::ChronoTrigger() :
	stopped(true), accumulated(0), trigger(0) { }

ChronoTrigger::ChronoTrigger(const unsigned int timeout) :
	stopped(false), accumulated(0), trigger(timeout) { }

bool ChronoTrigger::is_stopped()
{
	return this->stopped;
}

void ChronoTrigger::set_timeout(const unsigned int timeout)
{
	this->trigger = timeout;
	this->accumulated = 0;
}

void ChronoTrigger::start()
{
	R_ASSERT(this->trigger > 0, "No time out set");
	this->stopped = false;
}

void ChronoTrigger::stop()
{
	this->stopped = true;
}

void ChronoTrigger::tick()
{
	this->tick_impl();
}

#endif
