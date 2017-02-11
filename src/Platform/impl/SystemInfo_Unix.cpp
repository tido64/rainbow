// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"

#include <locale.h>
#include <unistd.h>

#include "Common/String.h"

namespace rainbow { namespace system_info
{
#ifdef RAINBOW_OS_UNIX
    bool has_accelerometer() { return false; }

    bool has_touchscreen() { return false; }

    auto locales() -> std::vector<std::string>
    {
        czstring lc = setlocale(LC_ALL, nullptr);
        if (lc == nullptr)
        {
            lc = setlocale(LC_MESSAGES, nullptr);
        }
        else
        {
            constexpr char kLCMessages[] = "LC_MESSAGES";
            czstring lc_msg = strstr(lc, kLCMessages);
            if (lc_msg != nullptr)
                lc = lc_msg + sizeof(kLCMessages);
        }

        std::vector<std::string> locales;
        if (lc == nullptr || (lc[0] == 'C' && lc[1] == '\0'))
        {
            locales.emplace_back("en");
        }
        else
        {
            czstring first = lc;
            for (czstring c = first; *c != '\0'; ++c)
            {
                if (*c == ';')
                {
                    if (c > first)
                        locales.emplace_back(first, c);
                    first = c + 1;
                }
            }
        }
        return locales;
    }
#endif

    auto memory() -> size_t
    {
        return sysconf(_SC_PAGE_SIZE) / 1024 *
               sysconf(_SC_PHYS_PAGES) / 1024;
    }
}}  // namespace rainbow::system_info
