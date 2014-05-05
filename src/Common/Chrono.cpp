// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Chrono.h"

#include <thread>

namespace
{
	Chrono::time_point now()
	{
		const auto now = Chrono::clock::now();
		return std::chrono::time_point_cast<Chrono::duration>(now);
	}
}

void Chrono::sleep(const duration::rep ms)
{
	std::this_thread::sleep_for(duration(ms));
}

Chrono::Chrono() : current_(now()), previous_(current_) { }

void Chrono::update()
{
	previous_ = current_;
	current_ = now();
	delta_ = std::chrono::duration_cast<duration>(current_ - previous_);
}
