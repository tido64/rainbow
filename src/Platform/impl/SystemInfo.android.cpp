// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

bool rainbow::system_info::has_accelerometer()
{
    return true;
}

bool rainbow::system_info::has_touchscreen()
{
    return true;
}

auto rainbow::system_info::locales() -> std::vector<std::string>
{
    return {"en"};
}
