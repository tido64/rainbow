#ifdef RAINBOW_IOS

/// Apple-implementation of Chrono.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Chrono : public Rainbow::_Chrono<Chrono>
{
public:
	Chrono() { }
	unsigned long get_time();
	void update(const unsigned int t);
};

#endif
