#ifndef THREAD_NOOPTASK_H_
#define THREAD_NOOPTASK_H_

#include "Thread/Task.h"

namespace Rainbow
{
	/// No-op task. Used for debugging purposes.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class NoopTask : public Task
	{
	public:
		virtual ~NoopTask() { }
		virtual void run() { }
	};
}

#endif
