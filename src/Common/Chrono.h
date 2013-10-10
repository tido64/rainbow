#ifndef COMMON_CHRONO_H_
#define COMMON_CHRONO_H_

#include <chrono>

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
	typedef std::chrono::steady_clock clock;
	typedef std::chrono::milliseconds duration;
	typedef clock::time_point time_point;

	static void sleep(const duration::rep milliseconds);

	Chrono();

	/// Returns timestamp of current frame.
	inline duration::rep current() const;

	/// Returns the time difference between current and previous frame.
	inline duration::rep delta() const;

	/// Updates the clock.
	void update();

protected:
	duration delta_;
	time_point current_;
	time_point previous_;
};

Chrono::duration::rep Chrono::current() const
{
	const auto t = this->current_.time_since_epoch();
	return std::chrono::duration_cast<duration>(t).count();
}

Chrono::duration::rep Chrono::delta() const
{
	return this->delta_.count();
}

#endif
