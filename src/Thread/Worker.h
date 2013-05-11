#ifndef THREAD_WORKER_H_
#define THREAD_WORKER_H_

#include <cstddef>

namespace Rainbow
{
	class Task;
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
		unsigned int count;  ///< Number of tasks completed. For debugging purposes.
		const size_t id;     ///< Worker id.
		Task *task;          ///< Task to execute. \c nullptr if terminating.
		ThreadPool *pool;    ///< The thread pool this worker belongs to.

		Worker(ThreadPool *pool, const size_t id);
	};
}

#endif
