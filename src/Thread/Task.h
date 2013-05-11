#ifndef THREAD_TASK_H_
#define THREAD_TASK_H_

#include "Common/NonCopyable.h"

namespace Rainbow
{
	/// A dispatchable task.
	///
	/// The main method to override here is \c run(). It will be executed on a
	/// worker thread. If post-processing on the main thread is required,
	/// \c end() should be overridden.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Task : private NonCopyable<Task>
	{
	public:
		virtual ~Task() { }

		/// End task. This is called on the main thread, after \c run().
		virtual void end() { }

		/// Run task. This is called on a worker thread.
		virtual void run() = 0;
	};
}

#endif
