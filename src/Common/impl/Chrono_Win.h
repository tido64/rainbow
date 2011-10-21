/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Chrono.h"

#if defined(RAINBOW_WIN) && !defined(RAINBOW_SDL)

/// Windows-implementation of Chrono.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Chrono : public Rainbow::_Chrono<Chrono>
{
public:
	Chrono();
	unsigned long get_time();

private:
	/// Intentionally left undefined.
	Chrono(const Chrono &);

	/// Intentionally left undefined.
	Chrono& operator=(const Chrono &);
};

#endif
