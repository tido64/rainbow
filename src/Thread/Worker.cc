// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Thread/ThreadPool.h"
#include "Thread/Worker.h"

namespace Rainbow
{
	Worker::Worker(ThreadPool *pool, const unsigned int id)
	    : count(0), id(id), pool(pool) { }

	Worker::~Worker()
	{
		if (this->count > 0)
			printf("Worker #%u did %u tasks\n", this->id, this->count);
	}

	void Worker::operator()()
	{
		while (true)
		{
			this->pool->report(*this);
			if (!this->task)
				break;
			this->task();
			++count;
		}
	}
}
