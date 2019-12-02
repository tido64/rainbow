// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_SYSTEMINFO_H_
#define PLATFORM_SYSTEMINFO_H_

#include <string>
#include <vector>

#include "Platform/Macros.h"

namespace rainbow::system_info
{
    /// <summary>
    ///   Returns whether the platform has an accelerometer.
    /// </summary>
    auto has_accelerometer() -> bool;

    /// <summary>Returns whether the platform has a touchscreen.</summary>
    auto has_touchscreen() -> bool;

    /// <summary>Returns a list of user-preferred locales.</summary>
    auto locales() -> std::vector<std::string>;

    /// <summary>
    ///   Returns total number of physical memory installed in megabytes.
    /// </summary>
    auto memory() -> size_t;
}  // namespace rainbow::system_info

#endif
