#ifndef THREAD_RWLOCK_H_
#define THREAD_RWLOCK_H_

#include "Platform/Macros.h"
#ifdef RAINBOW_WIN
#	include <windows.h>
#	define r_rwlock_lock           AcquireSRWLockExclusive
#	define r_rwlock_lock_shared    AcquireSRWLockShared
#	define r_rwlock_t              SRWLOCK
#	define r_rwlock_unlock         ReleaseSRWLockExclusive
#	define r_rwlock_unlock_shared  ReleaseSRWLockShared
#else
#	include <pthread.h>
#	define r_rwlock_lock           pthread_rwlock_wrlock
#	define r_rwlock_lock_shared    pthread_rwlock_rdlock
#	define r_rwlock_t              pthread_rwlock_t
#	define r_rwlock_unlock         pthread_rwlock_unlock
#	define r_rwlock_unlock_shared  pthread_rwlock_unlock
#endif

namespace Rainbow
{
	/// Wrapper around platform-implementation of readers/writer lock.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class RWLock
	{
	public:
	#ifdef RAINBOW_WIN
		RWLock() { InitializeSRWLock(&this->shared_lock); }
	#else
		RWLock() : shared_lock(PTHREAD_RWLOCK_INITIALIZER) { }
	#endif

		void lock()
		{
			r_rwlock_lock(&this->shared_lock);
		}

		void lock_shared()
		{
			r_rwlock_lock_shared(&this->shared_lock);
		}

		void unlock()
		{
			r_rwlock_unlock(&this->shared_lock);
		}

		void unlock_shared()
		{
			r_rwlock_unlock_shared(&this->shared_lock);
		}

	private:
		r_rwlock_t shared_lock;
	};
}

#undef r_rwlock_unlock_shared
#undef r_rwlock_unlock
#undef r_rwlock_t
#undef r_rwlock_lock_shared
#undef r_rwlock_lock
#endif
