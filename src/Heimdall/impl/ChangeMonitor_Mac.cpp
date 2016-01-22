// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/ChangeMonitor.h"

#include <memory>
#include <CoreFoundation/CoreFoundation.h>

#include "Common/Logging.h"
#include "Common/NonCopyable.h"

namespace
{
    const FSEventStreamEventFlags kFileModified =
        kFSEventStreamEventFlagItemModified | kFSEventStreamEventFlagItemIsFile;
    const CFTimeInterval kFSEventLatency = 0.5;

    void callback(ConstFSEventStreamRef /*streamRef*/,
                  void* clientCallBackInfo,
                  size_t numEvents,
                  void* eventPaths,
                  const FSEventStreamEventFlags eventFlags[],
                  const FSEventStreamEventId /*eventIds*/[])
    {
        auto monitor = static_cast<ChangeMonitor*>(clientCallBackInfo);
        auto paths = static_cast<char**>(eventPaths);
        for (size_t i = 0; i < numEvents; ++i)
        {
            if ((eventFlags[i] & kFileModified) != kFileModified)
                continue;

            monitor->on_modified(paths[i]);
        }
    }
}

ChangeMonitor::ChangeMonitor(const char* directory)
    : stream_(nullptr), callback_([](const char*) {})
{
    memset(&context_, 0, sizeof(context_));
    context_.info = this;

    CFStringRef path = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
                                                       directory,
                                                       kCFStringEncodingUTF8,
                                                       kCFAllocatorNull);
    CFArrayRef paths = CFArrayCreate(
        kCFAllocatorDefault, reinterpret_cast<const void**>(&path), 1, nullptr);
    stream_ = FSEventStreamCreate(kCFAllocatorDefault,
                                  ::callback,
                                  &context_,
                                  paths,
                                  kFSEventStreamEventIdSinceNow,
                                  kFSEventLatency,
                                  kFSEventStreamCreateFlagFileEvents);
    CFRelease(paths);
    CFRelease(path);

    FSEventStreamScheduleWithRunLoop(
        stream_, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    FSEventStreamStart(stream_);

    LOGI("Monitoring '%s'", directory);
}

ChangeMonitor::~ChangeMonitor()
{
    FSEventStreamStop(stream_);
    FSEventStreamInvalidate(stream_);
    FSEventStreamRelease(stream_);
}
