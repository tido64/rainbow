// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Chrono.h"

#include <thread>

using rainbow::Chrono;

namespace
{
    Chrono::clock::duration now()
    {
        return Chrono::clock::now().time_since_epoch();
    }
}

void Chrono::sleep(duration::rep milliseconds)
{
    std::this_thread::sleep_for(duration(milliseconds));
}

auto Chrono::system_now() -> Chrono::duration
{
    return std::chrono::duration_cast<duration>(
        std::chrono::system_clock::now().time_since_epoch());
}

auto Chrono::time_since_epoch() -> std::chrono::seconds
{
    return std::chrono::duration_cast<std::chrono::seconds>(now());
}

Chrono::Chrono() : delta_(0), current_(now()), previous_(current_)
{
}

void Chrono::tick()
{
    previous_ = current_;
    current_ = now();
    delta_ = std::chrono::duration_cast<duration>(current_ - previous_);
}
