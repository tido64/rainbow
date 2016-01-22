// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)

#include <sys/types.h>
#include <sys/sysctl.h>
#include <CoreFoundation/CoreFoundation.h>

namespace rainbow
{
    namespace system_info
    {
        bool has_accelerometer()
        {
#ifdef RAINBOW_OS_IOS
            return true;
#else
            return false;
#endif
        }

        bool has_touchscreen()
        {
#ifdef RAINBOW_OS_IOS
            return true;
#else
            return false;
#endif
        }

        void locales(std::vector<std::unique_ptr<char[]>>& locales)
        {
            char tmp[16];
            CFArrayRef localesArray = CFLocaleCopyPreferredLanguages();
            const long count = CFArrayGetCount(localesArray);
            locales.reserve(count);
            for (long i = 0; i < count; ++i)
            {
                CFStringRef lang = static_cast<CFStringRef>(
                    CFArrayGetValueAtIndex(localesArray, i));
                CFStringGetCString(
                    lang, tmp, sizeof(tmp), kCFStringEncodingUTF8);
                locales.emplace_back(new char[sizeof(tmp)]);
                strcpy(locales[i].get(), tmp);
            }
            CFRelease(localesArray);
        }

        size_t memory()
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
    }
}

#endif
