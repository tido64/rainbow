// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_CHANGEMONITOR_H_
#define HEIMDALL_CHANGEMONITOR_H_

#include <functional>

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_MACOS)
#    include <CoreServices/CoreServices.h>
#elif defined(RAINBOW_OS_WINDOWS)
#    include <future>

#    include <Windows.h>
#endif

#include "Common/NonCopyable.h"
#include "Common/String.h"

namespace heimdall
{
    class ChangeMonitor : private rainbow::NonCopyable<ChangeMonitor>
    {
    public:
        explicit ChangeMonitor(rainbow::czstring directory);
        ~ChangeMonitor();

        template <typename F>
        void set_callback(F&& callback)
        {
            callback_ = std::move(callback);
        }

        void on_modified(rainbow::czstring path) { callback_(path); }

    private:
#if defined(RAINBOW_OS_MACOS)
        FSEventStreamRef stream_;
        FSEventStreamContext context_;
#elif defined(RAINBOW_OS_WINDOWS)
        bool monitoring_;
        HANDLE hDirectory_;
        std::future<void> worker_;
#endif
        std::function<void(rainbow::czstring)> callback_;
    };
}  // namespace heimdall

#endif
