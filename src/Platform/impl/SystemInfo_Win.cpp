// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

#include <Windows.h>

namespace
{
    bool HasGetUserDefaultLocaleName()
    {
        HMODULE hModule = GetModuleHandle(TEXT("kernel32.dll"));
        return hModule != nullptr &&
               GetProcAddress(hModule, "GetUserDefaultLocaleName") != nullptr;
    }
}

namespace rainbow { namespace system_info
{
    bool has_accelerometer() { return false; }

    bool has_touchscreen() { return false; }

    auto locales() -> std::vector<std::string>
    {
        std::vector<std::string> locales{{}};

        if (!HasGetUserDefaultLocaleName())  // Windows 98, XP
        {
            const LCID lcid = GetUserDefaultLCID();
            char lc_data[LOCALE_NAME_MAX_LENGTH];
            int length =
                GetLocaleInfo(lcid, LOCALE_SISO639LANGNAME, lc_data, 9);
            lc_data[length++] = '-';
            GetLocaleInfo(lcid, LOCALE_SISO3166CTRYNAME, lc_data + length, 9);
            locales[0] = lc_data;
        }
        else  // Windows Vista and later
        {
            wchar_t wname[LOCALE_NAME_MAX_LENGTH];
            const int size =
                GetUserDefaultLocaleName(wname, LOCALE_NAME_MAX_LENGTH);
            if (size > 0)
            {
                char name[LOCALE_NAME_MAX_LENGTH];
                if (WideCharToMultiByte(CP_UTF8,
                                        0,
                                        wname,
                                        -1,
                                        name,
                                        sizeof(name),
                                        nullptr,
                                        nullptr) > 0)
                {
                    locales[0] = name;
                }
            }
        }

        return locales;
    }

    auto memory() -> size_t
    {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys / (1024 * 1024);
    }
}}  // namespace rainbow::system_info
