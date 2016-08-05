// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_CHRONO_H_
#define COMMON_CHRONO_H_

#include <chrono>

#include "Common/NonCopyable.h"

/// <summary>Simple class for keeping time.</summary>
/// <remarks>Resolution of time is in milliseconds.</remarks>
class Chrono : private NonCopyable<Chrono>
{
public:
    using clock = std::chrono::steady_clock;
    using duration = std::chrono::milliseconds;

    static void sleep(duration::rep milliseconds);
    static auto system_now() -> duration;
    static auto time_since_epoch() -> std::chrono::seconds;

    Chrono();

    /// <summary>
    ///   Returns the time difference between current and previous frame.
    /// </summary>
    auto delta() const { return delta_.count(); }

    /// <summary>Moves the clock one tick forward.</summary>
    void tick();

protected:
    duration delta_;
    clock::duration current_;
    clock::duration previous_;
};

#endif
