#ifndef CHRONO_H_
#define CHRONO_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

/// Simple class for keeping time.
///
/// Resolution of time is in milliseconds.
///
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Chrono : private NonCopyable<Chrono>
{
public:
	static void sleep(const unsigned int milliseconds);
	static unsigned long time();

	inline Chrono();

	/// Return timestamp of current frame.
	inline unsigned long current() const;

	/// Return the time difference between current and previous frame.
	inline unsigned long diff() const;

	/// Return timestamp of previous frame.
	inline unsigned long previous() const;

	/// Update the clock.
	void update();

protected:
	unsigned long tm_current;   ///< Current frame time in milliseconds.
	unsigned long tm_dt;        ///< Time difference between current and previous frame.
	unsigned long tm_previous;  ///< Previous frame time in milliseconds.
};

Chrono::Chrono() : tm_current(0), tm_dt(0), tm_previous(0) { }

unsigned long Chrono::current() const
{
	R_ASSERT(this->tm_current > 0, "Current time has not been updated");
	return this->tm_current;
}

unsigned long Chrono::diff() const
{
	return this->tm_dt;
}

unsigned long Chrono::previous() const
{
	R_ASSERT(this->tm_previous > 0, "Previous frame time has not been saved");
	return this->tm_previous;
}

void Chrono::update()
{
	this->tm_previous = this->tm_current;
	this->tm_current = this->time();
	this->tm_dt = this->tm_current - this->tm_previous;
}

#include "Common/impl/Chrono_SDL.h"
#include "Common/impl/Chrono_Unix.h"

#endif
