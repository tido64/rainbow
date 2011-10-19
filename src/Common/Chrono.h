#ifndef CHRONO_H_
#define CHRONO_H_

#include <cassert>

/// Simple class for keeping time.
///
/// Resolution of time is in milliseconds.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Chrono
{
public:
	static Chrono& Instance();

	/// Return current time.
	unsigned long current();

	/// Return last frame's time.
	unsigned long last_frame();

	/// Tick tock.
	void update();

private:
	unsigned long tm_current;  ///< Current frame time in milliseconds
	unsigned long tm_last;     ///< Last frame time in milliseconds

	Chrono();

	/// Intentionally left undefined.
	Chrono(const Chrono &);

	/// Platform-dependent function for retrieving current time in milliseconds.
	unsigned long get_time();

	/// Intentionally left undefined.
	Chrono& operator=(const Chrono &);
};

inline Chrono& Chrono::Instance()
{
	static Chrono chrono;
	return chrono;
}

inline unsigned long Chrono::current()
{
	assert(tm_current > 0 || !"Rainbow::Chrono: Current time has not been updated.");
	return tm_current;
}

inline unsigned long Chrono::last_frame()
{
	assert(tm_last > 0 || !"Rainbow::Chrono: Last frame time has not been saved.");
	return tm_last;
}

inline void Chrono::update()
{
	tm_last = tm_current;
	tm_current = this->get_time();
}   

#endif
