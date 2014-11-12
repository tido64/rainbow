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

	static void sleep(const duration::rep milliseconds);
	static duration system_now();
	static std::chrono::seconds time_since_epoch();

	Chrono();

	/// Returns the time difference between current and previous frame.
	inline duration::rep delta() const;

	/// Updates the clock.
	void update();

protected:
	duration delta_;
	clock::duration current_;
	clock::duration previous_;
};

Chrono::duration::rep Chrono::delta() const
{
	return delta_.count();
}

#endif
