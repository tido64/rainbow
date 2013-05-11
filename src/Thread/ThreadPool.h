#ifndef THREAD_THREADPOOL_H_
#define THREAD_THREADPOOL_H_

#include <thread>

#include "Common/Vector.h"
#include "Thread/RWLock.h"
#include "Thread/Semaphore.h"

namespace Rainbow
{
	class Task;
	class Worker;

	/// Thread pool. Dispatched tasks are run as soon as a thread is available
	/// and may or may not be completed upon return. The only way to make sure
	/// tasks are completed is to call \c finish(). Tasks are owned by the
	/// caller.
	///
	/// \see http://en.wikipedia.org/wiki/Read_write_lock_pattern
	/// \see http://en.wikipedia.org/wiki/Spurious_wakeup
	/// \see http://en.wikipedia.org/wiki/Thread_pool_pattern
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class ThreadPool
	{
	public:
		/// Return number of hardware threads available on the current system
		/// (e.g. number of CPUs or cores or hyperthreading units), or 0 if this
		/// information is not available.
		static unsigned int hardware_concurrency();

		/// Construct a pool of \p num_threads threads.
		ThreadPool(const size_t num_threads);
		~ThreadPool();

		/// Clear the task queue. Does not delete the task objects.
		void clear();

		/// Dispatch a task to a worker thread. Task must stay alive until the
		/// main thread has returned from \c finish() or the task queued has
		/// been cleared.
		void dispatch(Task *task);

		/// Wait for all tasks to finish. Task queue is cleared and individual
		/// tasks can be safely deleted when this call returns.
		void finish();

		/// Called by a worker to hand in a task and get or wait for a new one.
		void report(Worker *thread);

		/// Pre-allocate memory for \p num_tasks tasks in queue.
		void reserve(const size_t num_tasks);

	private:
		bool terminate;                 ///< Whether the thread pool is being destructed.
		std::atomic<size_t> finished;   ///< Number of finished tasks.
		std::atomic<size_t> next_task;  ///< Next task to grab.
		const size_t num_threads;       ///< Number of threads in the pool.
		std::thread *threads;           ///< Pool of threads.
		RWLock queue;                   ///< Readers/writer lock for locking the task queue.
		Semaphore taskqueue;            ///< Semaphore to wake up threads when tasks are queued.
		Vector<Task*> tasks;            ///< Tasks in queue.

		/// Workaround for the case where some threads are put on hold for other
		/// threads and these finish quickly, bringing the semaphore down to 0
		/// (or below), preventing earlier threads from ever waking up to run
		/// their task. Run before and after a wait for signal to make sure a
		/// thread is not holding back other threads.
		void post_if_starved();
	};
}

#endif
