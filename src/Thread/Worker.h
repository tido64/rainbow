#ifndef THREAD_WORKER_H_
#define THREAD_WORKER_H_

#include <functional>

namespace Rainbow
{
	typedef std::function<void()> Task;

	class ThreadPool;

	/// Worker perform tasks in a thread.
	///
	/// Reports to the thread pool and waits for a task. Once received, it
	/// completes the task and reports back to the thread pool and await a new
	/// one.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Worker
	{
		friend class ThreadPool;

	public:
		~Worker();
		void operator()();

	private:
		unsigned int count;     ///< Number of tasks completed. For debugging purposes.
		const unsigned int id;  ///< Worker id.
		ThreadPool *pool;       ///< The thread pool this worker belongs to.
		Task task;              ///< Task to execute. \c nullptr if terminating.

		Worker(ThreadPool *pool, const unsigned int id);
	};
}

#endif
