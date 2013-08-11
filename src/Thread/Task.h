#ifndef THREAD_TASK_H_
#define THREAD_TASK_H_

#include "Common/NonCopyable.h"

namespace Rainbow
{
	/// A dispatchable task.
	///
	/// The main method to override here is \c run_impl(). It will be executed
	/// on a worker thread. If post-processing on the main thread is required,
	/// \c end_impl() should be overridden.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Task : private NonCopyable<Task>
	{
	public:
		virtual ~Task() { }

		/// Ends task. This is called on the main thread, after \c run().
		inline void end();

		/// Runs task. This is called on a worker thread.
		inline void run();

	private:
		virtual void end_impl() { }
		virtual void run_impl() = 0;
	};

	void Task::end()
	{
		this->end_impl();
	}

	void Task::run()
	{
		this->run_impl();
	}
}

#endif
