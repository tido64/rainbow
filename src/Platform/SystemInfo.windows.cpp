// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

#include <Windows.h>

auto rainbow::system_info::has_accelerometer() -> bool
{
    return false;
}

auto rainbow::system_info::has_touchscreen() -> bool
{
    return GetSystemMetrics(SM_MAXIMUMTOUCHES) > 0;
}

auto rainbow::system_info::locales() -> std::vector<std::string>
{
    std::vector<std::string> locales{{}};

    wchar_t wname[LOCALE_NAME_MAX_LENGTH];
    const int size = GetUserDefaultLocaleName(wname, LOCALE_NAME_MAX_LENGTH);
    if (size > 0) {
        char name[LOCALE_NAME_MAX_LENGTH];
        if (WideCharToMultiByte(
                CP_UTF8, 0, wname, -1, name, sizeof(name), nullptr, nullptr) >
            0) {
            locales[0] = name;
        }
    }

    return locales;
}

auto rainbow::system_info::memory() -> size_t
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys / (1024 * 1024);
}
