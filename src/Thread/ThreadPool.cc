// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <algorithm>

#include "Thread/ThreadPool.h"
#include "Thread/Worker.h"

namespace Rainbow
{
	unsigned int ThreadPool::recommended_pool_size()
	{
		const unsigned int num_threads = std::thread::hardware_concurrency();
		return (num_threads == 0) ? 1 : num_threads - 1;
	}

	ThreadPool::ThreadPool(const unsigned int num_threads)
	    : terminate(false), num_threads(num_threads), next_task(0), taskqueue(0)
	{
		R_DEBUG("[Rainbow] Number of hardware threads: %u\n",
		        std::thread::hardware_concurrency());

		this->threads.reset(new std::thread[this->num_threads]);
		for (unsigned int i = 0; i < this->num_threads; ++i)
			this->threads[i] = std::thread(Worker(this, i + 2));
	}

	ThreadPool::~ThreadPool()
	{
		this->terminate = true;
		this->taskqueue.post_all();
		for (unsigned int i = 0; i < this->num_threads; ++i)
			this->threads[i].join();
	}

	void ThreadPool::clear()
	{
		this->tasks.clear();
		this->next_task = 0;
	}

	void ThreadPool::dispatch(const Task &task)
	{
		if (this->tasks.size() == this->tasks.capacity())
		{
			// Lock access to the task queue while the vector resizes.
			this->queue.lock();
			this->tasks.push_back(task);
			this->queue.unlock();
		}
		else
			this->tasks.push_back(task);
		this->taskqueue.post();
	}

	void ThreadPool::finish()
	{
		// Run a few tasks while waiting for all threads to return.
		while (this->taskqueue != static_cast<int>(this->num_threads) * -1)
		{
			for (size_t t = this->next_task++; t < this->tasks.size(); t = this->next_task++)
				this->tasks[t]();
		}
		this->clear();
	}

	void ThreadPool::report(Worker &worker)
	{
		if (this->terminate)
		{
			this->taskqueue.post();
			worker.task = nullptr;
			return;
		}
		size_t t = this->next_task++;
		while (t >= this->tasks.size())
		{
			this->post_if_starved();
			this->taskqueue.wait();
			if (this->terminate)
			{
				this->taskqueue.post();
				worker.task = nullptr;
				return;
			}
			// Check whether the task queue was emptied.
			if (t >= this->next_task)
				t = this->next_task++;
		}
		this->post_if_starved();
		this->queue.lock_shared();
		worker.task = this->tasks[t];
		this->queue.unlock_shared();
	}

	void ThreadPool::reserve(const size_t num_tasks)
	{
		this->tasks.reserve(num_tasks);
	}

	void ThreadPool::post_if_starved()
	{
		if (this->taskqueue < 0 && this->next_task < this->tasks.size())
			this->taskqueue.post();
	}
}
