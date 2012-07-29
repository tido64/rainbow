#ifndef CHRONOTRIGGER_H_
#define CHRONOTRIGGER_H_

#include "Common/Chrono.h"

/// A ticker with a configurable time interval.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class ChronoTrigger
{
public:
	inline ChronoTrigger();

	inline ChronoTrigger(const unsigned int timeout);

	virtual ~ChronoTrigger();

	/// Whether this ticker is running.
	inline bool is_stopped();

	/// Set time till a tick.
	inline void set_timeout(const unsigned int timeout);

	/// Start accumulating time.
	inline void start();

	/// Stop accumulating time.
	inline void stop();

	/// Called every tick.
	virtual void tick() = 0;

	/// Accumulate time and trigger when it reaches time out.
	void update();

private:
	bool stopped;              ///< Whether time is accumulating.
	unsigned int accumulated;  ///< Accumulated, monotonic time.
	unsigned int trigger;      ///< Time till a tick.
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

#endif
