// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef HEIMDALL_CHANGEMONITOR_H_
#define HEIMDALL_CHANGEMONITOR_H_

#ifdef USE_HEIMDALL

#include <functional>

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_MACOS)
#	include <CoreServices/CoreServices.h>
#elif defined(RAINBOW_OS_WINDOWS)
#	include <future>
#	include <Windows.h>
#endif

#include "Common/NonCopyable.h"

struct lua_State;

class ChangeMonitor : private NonCopyable<ChangeMonitor>
{
public:
	typedef std::function<void(const char *)> Callback;

	explicit ChangeMonitor(const char *const directory);
	~ChangeMonitor();

	inline void set_callback(Callback &&callback);

	inline void on_modified(const char *path);

private:
#if defined(RAINBOW_OS_MACOS)
	FSEventStreamRef stream;
	FSEventStreamContext context;
#elif defined(RAINBOW_OS_WINDOWS)
	bool monitoring;
	HANDLE hDirectory;
	std::future<void> worker;
#endif
	Callback callback;
};

void ChangeMonitor::set_callback(Callback &&callback)
{
	this->callback = callback;
}

void ChangeMonitor::on_modified(const char *path)
{
	this->callback(path);
}

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_CHANGEMONITOR_H_
