/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Chrono.h"

#if defined(RAINBOW_UNIX) && !defined(RAINBOW_SDL)
#if _POSIX_TIMERS <= 0
#	warning "POSIX Realtime extension not found."
#	include <sys/time.h>
#else
#	include <ctime>
#endif
#include <unistd.h>

/// Unix-implementation of Chrono.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Chrono : public Rainbow::_Chrono<Chrono>
{
public:
	Chrono() { }
	unsigned long get_time();

private:
	/// Intentionally left undefined.
	Chrono(const Chrono &);

	/// Intentionally left undefined.
	Chrono& operator=(const Chrono &);
};

#endif
