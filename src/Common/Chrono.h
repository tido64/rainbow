#ifndef CHRONO_H_
#define CHRONO_H_

#include "Common/Debug.h"
#include "Common/NonCopyable.h"

namespace Rainbow
{
	/// Simple class for keeping time.
	///
	/// Resolution of time is in milliseconds.
	///
	/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	template<class T>
	class ChronoBase : private NonCopyable<ChronoBase<T> >
	{
	public:
		static void sleep(const unsigned int milliseconds);

		inline ChronoBase();

		/// Return timestamp of current frame.
		inline unsigned long current();

		/// Return the time difference between current and previous frame.
		inline unsigned long diff();

		/// Return timestamp of previous frame.
		inline unsigned long previous();

		/// Update the clock.
		void update();

	protected:
		unsigned long tm_current;   ///< Current frame time in milliseconds.
		unsigned long tm_dt;        ///< Time difference between current and previous frame.
		unsigned long tm_previous;  ///< Previous frame time in milliseconds.

		T get_time;  ///< Platform-specific method for getting monotonic time.
	};

	template<class T>
	ChronoBase<T>::ChronoBase() : tm_current(0), tm_dt(0), tm_previous(0) { }

	template<class T>
	unsigned long ChronoBase<T>::current()
	{
		R_ASSERT(this->tm_current > 0, "Current time has not been updated");
		return this->tm_current;
	}

	template<class T>
	unsigned long ChronoBase<T>::diff()
	{
		return this->tm_dt;
	}

	template<class T>
	unsigned long ChronoBase<T>::previous()
	{
		R_ASSERT(this->tm_previous > 0, "Previous frame time has not been saved");
		return this->tm_previous;
	}

	template<class T>
	void ChronoBase<T>::update()
	{
		this->tm_previous = this->tm_current;
		this->tm_current = this->get_time();
		this->tm_dt = this->tm_current - this->tm_previous;
	}
}

#include "Common/impl/Chrono_Apple.h"
#include "Common/impl/Chrono_SDL.h"
#include "Common/impl/Chrono_Unix.h"

#endif
