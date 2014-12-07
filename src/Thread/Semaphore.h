// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THREAD_SEMAPHORE_H_
#define THREAD_SEMAPHORE_H_

#include <atomic>
#include <condition_variable>
#include <mutex>

namespace Rainbow
{
	/// Fast semaphore implementation. Avoids calls to kernel space when there
	/// is no need to change a thread's state.
	///
	/// \see http://cbloomrants.blogspot.no/2011/07/07-09-11-lockfree-thomasson-simple-mpmc.html
	/// \see http://cbloomrants.blogspot.no/2011/12/12-08-11-some-semaphores.html
	/// \see http://www.haiku-os.org/legacy-docs/benewsletter/Issue1-26.html
	class Semaphore
	{
	public:
		Semaphore() : count_(0) {}

		void consume_one()
		{
			--count_;
		}

		void post()
		{
			if (++count_ <= 0)
				cond_.notify_one();
		}

		void post_all()
		{
			cond_.notify_all();
		}

		template<typename... Args>
		void wait(Args... args)
		{
			if (--count_ < 0)
			{
				std::unique_lock<std::mutex> lock(mutex_);
				cond_.wait(lock, std::forward<Args>(args)...);
			}
		}

		operator int() const
		{
			return count_;
		}

	private:
		std::atomic_int count_;
		std::condition_variable cond_;
		std::mutex mutex_;
	};
}

#endif
