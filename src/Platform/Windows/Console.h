// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef PLATFORM_WINDOWS_CONSOLE_H_
#define PLATFORM_WINDOWS_CONSOLE_H_

#include <cstdio>

namespace rainbow::windows
{
    class Console
    {
    public:
        Console();
        ~Console();

    private:
        bool is_attached_ = false;
        FILE* stdout_handle_ = nullptr;
    };
}  // namespace rainbow::windows

#endif
