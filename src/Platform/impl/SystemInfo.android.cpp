// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

auto rainbow::system_info::has_accelerometer() -> bool
{
    return true;
}

auto rainbow::system_info::has_touchscreen() -> bool
{
    return true;
}

auto rainbow::system_info::locales() -> std::vector<std::string>
{
    return {"en"};
}
