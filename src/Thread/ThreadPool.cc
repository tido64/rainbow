// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "Thread/Task.h"
#include "Thread/ThreadPool.h"
#include "Thread/Worker.h"

namespace Rainbow
{
	unsigned int ThreadPool::hardware_concurrency()
	{
		return std::thread::hardware_concurrency();
	}

	ThreadPool::ThreadPool(const size_t num_threads) :
		terminate(false), finished(0), next_task(0), num_threads(num_threads),
		threads(nullptr), taskqueue(0)
	{
		this->threads = new std::thread[this->num_threads];
		for (size_t i = 0; i < this->num_threads; ++i)
			this->threads[i] = std::thread(Worker(this, i + 2));
	}

	ThreadPool::~ThreadPool()
	{
		this->terminate = true;
		this->taskqueue.post_all();
		for (size_t i = 0; i < this->num_threads; ++i)
			this->threads[i].join();
		delete[] this->threads;
	}

	void ThreadPool::clear()
	{
		this->tasks.clear();
		this->next_task = 0;
		this->finished = 0;
	}

	void ThreadPool::dispatch(Task *task)
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
		// Run post on finished tasks while waiting for all threads to return.
		for (size_t i = 0; i < this->tasks.size();)
		{
			for (; i < this->finished; ++i)
				this->tasks[i]->end();
			// If the main thread can pick up a task, do so.
			if (this->next_task < this->tasks.size())
			{
				const size_t t = this->next_task++;
				if (t < this->tasks.size())
				{
					this->tasks[t]->run();
					++this->finished;
				}
			}
		}
		this->clear();
	}

	void ThreadPool::report(Worker *worker)
	{
		if (this->terminate)
		{
			this->taskqueue.post();
			worker->task = nullptr;
			return;
		}
		if (worker->task)
			++this->finished;
		size_t t = this->next_task++;
		while (t >= this->tasks.size())
		{
			this->post_if_starved();
			this->taskqueue.wait();
			this->post_if_starved();
			if (this->terminate)
			{
				this->taskqueue.post();
				worker->task = nullptr;
				return;
			}
			// Check whether the task queue was emptied.
			if (t >= this->next_task)
				t = this->next_task++;
		}
		this->queue.lock_shared();
		worker->task = this->tasks[t];
		this->queue.unlock_shared();
	}

	void ThreadPool::reserve(const size_t num_tasks)
	{
		this->tasks.reserve(num_tasks);
	}

	void ThreadPool::post_if_starved()
	{
		if (this->taskqueue < 0 && this->finished < this->tasks.size())
			this->taskqueue.post();
	}
}
