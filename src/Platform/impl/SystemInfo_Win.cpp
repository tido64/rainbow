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

    void locales(std::vector<std::unique_ptr<char[]>>& locales)
    {
        char* locale_id;
        if (!HasGetUserDefaultLocaleName())  // Windows 98, XP
        {
            LCID locale = GetUserDefaultLCID();
            char lpLCData[LOCALE_NAME_MAX_LENGTH];
            int length = GetLocaleInfo(
                locale, LOCALE_SISO639LANGNAME, lpLCData, 9);
            lpLCData[length++] = '-';
            length += GetLocaleInfo(
                locale, LOCALE_SISO3166CTRYNAME, lpLCData + length, 9);
            locale_id = new char[length + 1];
            memcpy(locale_id, lpLCData, length + 1);
        }
        else  // Windows Vista and later
        {
            wchar_t name[LOCALE_NAME_MAX_LENGTH];
            int size = GetUserDefaultLocaleName(name, LOCALE_NAME_MAX_LENGTH);
            if (size <= 0)
                return;

            size = WideCharToMultiByte(
                CP_UTF8, 0, name, -1, nullptr, 0, nullptr, nullptr);
            if (size <= 0)
                return;

            locale_id = new char[size];
            WideCharToMultiByte(
                CP_UTF8, 0, name, -1, locale_id, size, nullptr, nullptr);
        }
        locales.emplace_back(locale_id);
    }

    auto memory() -> size_t
    {
        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        return status.ullTotalPhys / (1024 * 1024);
    }
}}  // namespace rainbow::system_info
