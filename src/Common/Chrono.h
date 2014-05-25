// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_CHRONO_H_
#define COMMON_CHRONO_H_

#include <chrono>

#include "Common/NonCopyable.h"

/// Simple class for keeping time.
///
/// Resolution of time is in milliseconds.
class Chrono : private NonCopyable<Chrono>
{
public:
	using clock = std::chrono::steady_clock;
	using duration = std::chrono::milliseconds;
	using time_point = clock::time_point;

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
	const auto t = current_.time_since_epoch();
	return std::chrono::duration_cast<duration>(t).count();
}

Chrono::duration::rep Chrono::delta() const
{
	return delta_.count();
}

#endif
