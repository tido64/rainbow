// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

#include <array>
#include <sys/sysctl.h>
#include <sys/types.h>

#include <CoreFoundation/CoreFoundation.h>

auto rainbow::system_info::has_accelerometer() -> bool
{
#ifdef RAINBOW_OS_IOS
    return true;
#else
    return false;
#endif
}

auto rainbow::system_info::has_touchscreen() -> bool
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

    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    std::array<char, 16> tmp;

    for (int i = 0; i < count; ++i) {
        auto lang = static_cast<CFStringRef>(
            CFArrayGetValueAtIndex(preferred_langs, i));
        CFStringGetCString(lang, tmp.data(), tmp.size(), kCFStringEncodingUTF8);
        locales.emplace_back(tmp.data());
    }

    CFRelease(preferred_langs);
    return locales;
}

auto rainbow::system_info::memory() -> size_t
{
    static_assert(sizeof(size_t) >= sizeof(int64_t));

    std::array<int, 2> mib{CTL_HW, HW_MEMSIZE};
    int64_t memsize;
    size_t length = sizeof(memsize);
    sysctl(mib.data(), mib.size(), &memsize, &length, nullptr, 0);
    return memsize / (1024 * 1024);
}
