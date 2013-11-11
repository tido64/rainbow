#ifndef HEIMDALL_CHANGEMONITOR_H_
#define HEIMDALL_CHANGEMONITOR_H_

#ifdef USE_HEIMDALL

#include <functional>

#include "Platform/Macros.h"
#if defined(RAINBOW_MAC)
#	include <CoreServices/CoreServices.h>
#elif defined(RAINBOW_WIN)
#	include <future>
#	include <Windows.h>
#endif

#include "Common/NonCopyable.h"

struct lua_State;

class ChangeMonitor : private NonCopyable<ChangeMonitor>
{
public:
	typedef std::function<void(const char *)> Callback;

	ChangeMonitor(const char *const directory);
	~ChangeMonitor();

	inline void set_callback(Callback &&callback);

	inline void on_modified(const char *path);

private:
#if defined(RAINBOW_MAC)
	FSEventStreamRef stream;
	FSEventStreamContext context;
#elif defined(RAINBOW_WIN)
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
