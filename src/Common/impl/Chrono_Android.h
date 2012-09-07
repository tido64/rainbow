#ifdef RAINBOW_ANDROID

/// Android-implementation of Chrono.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Chrono : public Rainbow::_Chrono<Chrono>
{
public:
	inline Chrono();
	unsigned long get_time();
};

Chrono::Chrono() { }

#endif
