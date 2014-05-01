// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THREAD_THREADPOOL_H_
#define THREAD_THREADPOOL_H_

#include <functional>
#include <memory>
#include <thread>

#include "Common/Vector.h"
#include "Thread/Semaphore.h"

namespace Rainbow
{
	typedef std::function<void()> Task;

	/// Thread pool. Dispatched tasks are run as soon as a thread is available
	/// and may or may not be completed upon return. The only way to make sure
	/// tasks are completed is to call \c finish().
	///
	/// \see http://en.wikipedia.org/wiki/Read_write_lock_pattern
	/// \see http://en.wikipedia.org/wiki/Spurious_wakeup
	/// \see http://en.wikipedia.org/wiki/Thread_pool_pattern
	class ThreadPool
	{
	public:
		/// Returns number of hardware threads available on the current system
		/// (e.g. number of CPUs or cores or hyperthreading units) minus 1.
		static unsigned int recommended_pool_size();

		/// Constructs a pool of \p num_threads threads.
		ThreadPool(const unsigned int num_threads = recommended_pool_size());
		~ThreadPool();

		/// Clears the task queue.
		void clear();

		/// Dispatches a task to a worker thread.
		void dispatch(const Task &task);

		/// Waits for all tasks to finish. Task queue is cleared after this call.
		void finish();

		/// Pre-allocates memory for \p num_tasks tasks in queue.
		void reserve(const size_t num_tasks);

		/// Called by a worker to get or wait for a new task.
		Task wait_for_work();

	private:
		Vector<Task> tasks_;                      ///< Tasks in queue.
		std::atomic_uint next_task_;              ///< Next task to grab.
		bool shutting_down_;                      ///< Whether the thread pool is being shut down.
		std::mutex mutex_;                        ///< Task queue mutex.
		const unsigned int num_threads_;          ///< Number of threads in the pool.
		Semaphore semaphore_;                     ///< Semaphore to wake up threads when tasks are queued.
		std::unique_ptr<std::thread[]> threads_;  ///< Pool of threads.
	};
}

#endif
