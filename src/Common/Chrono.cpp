// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Chrono.h"

#include <thread>

namespace
{
    Chrono::clock::duration now()
    {
        return Chrono::clock::now().time_since_epoch();
    }
}

Chrono::duration Chrono::system_now()
{
    return std::chrono::duration_cast<Chrono::duration>(
        std::chrono::system_clock::now().time_since_epoch());
}

void Chrono::sleep(duration::rep ms)
{
    std::this_thread::sleep_for(duration(ms));
}

std::chrono::seconds Chrono::time_since_epoch()
{
    return std::chrono::duration_cast<std::chrono::seconds>(now());
}

Chrono::Chrono() : delta_(0), current_(now()), previous_(current_) {}

void Chrono::update()
{
    previous_ = current_;
    current_ = now();
    delta_ = std::chrono::duration_cast<duration>(current_ - previous_);
}
