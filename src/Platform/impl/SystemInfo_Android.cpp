// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

namespace rainbow { namespace system_info
{
    bool has_accelerometer() { return true; }

    bool has_touchscreen() { return true; }

    auto locales() -> std::vector<std::string>
    {
        return {"en"};
    }
}}  // namespace rainbow::system_info
