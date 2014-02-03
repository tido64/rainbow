// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_HEIMDALL

#include "Heimdall/ChangeMonitor.h"
#ifdef RAINBOW_OS_MACOS

#include <memory>
#include <CoreFoundation/CoreFoundation.h>

#include "Common/Debug.h"
#include "Common/NonCopyable.h"
#include "Lua/LuaHelper.h"

namespace
{
	const FSEventStreamEventFlags kFileModified = kFSEventStreamEventFlagItemModified
	                                            | kFSEventStreamEventFlagItemIsFile;
	const CFTimeInterval kFSEventLatency = 0.5;

	void callback(ConstFSEventStreamRef /*streamRef*/,
	              void *clientCallBackInfo,
	              size_t numEvents,
	              void *eventPaths,
	              const FSEventStreamEventFlags eventFlags[],
	              const FSEventStreamEventId /*eventIds*/[])
	{
		ChangeMonitor *monitor = static_cast<ChangeMonitor*>(clientCallBackInfo);
		const char *const *paths = static_cast<char**>(eventPaths);
		for (size_t i = 0; i < numEvents; ++i)
		{
			if ((eventFlags[i] & kFileModified) != kFileModified)
				continue;

			monitor->on_modified(paths[i]);
		}
	}
}

ChangeMonitor::ChangeMonitor(const char *const directory)
    : stream(nullptr), callback([](const char *) { })
{
	memset(&this->context, 0, sizeof(this->context));
	this->context.info = this;

	CFStringRef path = CFStringCreateWithCStringNoCopy(kCFAllocatorDefault,
	                                                   directory,
	                                                   kCFStringEncodingUTF8,
	                                                   kCFAllocatorNull);
	CFArrayRef paths = CFArrayCreate(kCFAllocatorDefault,
	                                 reinterpret_cast<const void**>(&path),
	                                 1,
	                                 nullptr);
	this->stream = FSEventStreamCreate(kCFAllocatorDefault,
	                                   ::callback,
	                                   &this->context,
	                                   paths,
	                                   kFSEventStreamEventIdSinceNow,
	                                   kFSEventLatency,
	                                   kFSEventStreamCreateFlagFileEvents);
	CFRelease(paths);
	CFRelease(path);

	FSEventStreamScheduleWithRunLoop(this->stream,
	                                 CFRunLoopGetCurrent(),
	                                 kCFRunLoopDefaultMode);
	FSEventStreamStart(this->stream);

	R_DEBUG("[Rainbow] Monitoring %s\n", directory);
}

ChangeMonitor::~ChangeMonitor()
{
	FSEventStreamStop(this->stream);
	FSEventStreamInvalidate(this->stream);
	FSEventStreamRelease(this->stream);
}

#endif  // RAINBOW_OS_MACOS
#endif  // USE_HEIMDALL
