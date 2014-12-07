// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Thread/Worker.h"

#include "Thread/ThreadPool.h"

namespace Rainbow
{
	Worker::Worker(ThreadPool *pool, const unsigned int id)
	    : count_(0), id_(id), pool_(pool) {}

	Worker::~Worker()
	{
		if (count_ > 0)
			printf("Worker #%u did %u tasks\n", id_, count_);
	}

	void Worker::operator()()
	{
		while (true)
		{
			task_ = pool_->wait_for_work();
			if (!task_)
				break;
			task_();
			++count_;
		}
	}
}
