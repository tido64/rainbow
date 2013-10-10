#include <thread>

#include "Common/Chrono.h"

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
	this->previous_ = this->current_;
	this->current_ = now();
	const auto dt = this->current_ - this->previous_;
	this->delta_ = std::chrono::duration_cast<duration>(dt);
}
