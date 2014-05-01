// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
	class Worker
	{
		friend ThreadPool;

	public:
		~Worker();
		void operator()();

	private:
		unsigned int count_;     ///< Number of tasks completed. For debugging purposes.
		const unsigned int id_;  ///< Worker id.
		ThreadPool *pool_;       ///< The thread pool this worker belongs to.
		Task task_;              ///< Task to execute. \c nullptr if terminating.

		Worker(ThreadPool *pool, const unsigned int id);
	};
}

#endif
