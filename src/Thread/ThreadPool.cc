// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Thread/ThreadPool.h"

#include <algorithm>

#include "Common/Vector.h"
#include "Thread/Worker.h"

namespace Rainbow
{
	unsigned int ThreadPool::recommended_pool_size()
	{
		const auto num_threads = std::thread::hardware_concurrency();
		return (num_threads == 0) ? 1 : num_threads - 1;
	}

	ThreadPool::ThreadPool(const unsigned int num_threads)
	    : next_task_(0), shutting_down_(false), num_threads_(num_threads),
	      threads_(new std::thread[num_threads_])
	{
		LOGI("Number of hardware threads: %u",
		     std::thread::hardware_concurrency());

		unsigned int i = 1;
		std::generate_n(threads_.get(), num_threads_, [this, &i]() {
			return std::thread(Worker(this, ++i));
		});
	}

	ThreadPool::~ThreadPool()
	{
		shutting_down_ = true;
		semaphore_.post_all();
		std::for_each(threads_.get(),
		              threads_.get() + num_threads_,
		              [](std::thread &t) { t.join(); });
	}

	void ThreadPool::clear()
	{
		tasks_.clear();
		next_task_ = 0;
	}

	void ThreadPool::dispatch(const Task &task)
	{
		if (tasks_.size() == tasks_.capacity())
		{
			// Lock access to the task queue while the vector resizes.
			std::lock_guard<std::mutex> lock(mutex_);
			tasks_.push_back(task);
		}
		else
			tasks_.push_back(task);
		semaphore_.post();
	}

	void ThreadPool::finish()
	{
		// Run a few tasks while waiting for all threads to return.
		const int idle = -static_cast<int>(num_threads_);
		while (semaphore_ != idle)
		{
			for (size_t t = next_task_++; t < tasks_.size(); t = next_task_++)
			{
				semaphore_.consume_one();
				tasks_[t]();
			}
		}
		clear();
	}

	void ThreadPool::reserve(const size_t num_tasks)
	{
		tasks_.reserve(num_tasks);
	}

	Task ThreadPool::wait_for_work()
	{
		if (shutting_down_)
		{
			semaphore_.post();
			return {};
		}
		size_t t = next_task_++;
		// Ensure thread doesn't wake up while there are no tasks. This may
		// occur because the main thread consumes tasks before decrementing the
		// semaphore.
		while (t >= tasks_.size())
		{
			semaphore_.wait([this, &t] {
				if (t >= next_task_)
					t = next_task_++;
				return shutting_down_ || t < tasks_.size();
			});
			if (shutting_down_)
			{
				semaphore_.post();
				return {};
			}
		}
		std::lock_guard<std::mutex> lock(mutex_);
		return tasks_[t];
	}
}
