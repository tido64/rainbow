// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

#include <sys/sysctl.h>
#include <sys/types.h>

#include <CoreFoundation/CoreFoundation.h>

bool rainbow::system_info::has_accelerometer()
{
#ifdef RAINBOW_OS_IOS
    return true;
#else
    return false;
#endif
}

bool rainbow::system_info::has_touchscreen()
{
#ifdef RAINBOW_OS_IOS
    return true;
#else
    return false;
#endif
}

auto rainbow::system_info::locales() -> std::vector<std::string>
{
    std::vector<std::string> locales;
    CFArrayRef preferred_langs = CFLocaleCopyPreferredLanguages();
    const int count = CFArrayGetCount(preferred_langs);
    locales.reserve(count);
    char tmp[16];
    for (int i = 0; i < count; ++i)
    {
        auto lang = static_cast<CFStringRef>(
            CFArrayGetValueAtIndex(preferred_langs, i));
        CFStringGetCString(lang, tmp, sizeof(tmp), kCFStringEncodingUTF8);
        locales.emplace_back(tmp);
    }
    CFRelease(preferred_langs);
    return locales;
}

auto rainbow::system_info::memory() -> size_t
{
    int mib[2]{CTL_HW, HW_MEMSIZE};
    int64_t memsize;
    size_t length = sizeof(memsize);
    sysctl(mib, 2, &memsize, &length, nullptr, 0);
    memsize /= 1024 * 1024;
    if (sizeof(size_t) < sizeof(int64_t))
    {
        const size_t memory = std::numeric_limits<size_t>::max();
        return (memsize < static_cast<int64_t>(memory)
                    ? static_cast<size_t>(memsize)
                    : memory);
    }
    return static_cast<size_t>(memsize);
}
