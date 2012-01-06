#ifndef CHRONO_H_
#define CHRONO_H_

#include <cassert>

namespace Rainbow
{
	/// Simple class for keeping time.
	///
	/// Resolution of time is in milliseconds.
	///
	/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	template<class T>
	class _Chrono
	{
	public:
		static T& Instance();

		virtual ~_Chrono() { }

		/// Return timestamp of current frame.
		unsigned long current();

		/// Return the time difference between current and previous frame.
		unsigned long diff();

		/// Return timestamp of previous frame.
		unsigned long previous();

		/// Update the clock using an external or internal source, depending on
		/// the platform.
		void update(const unsigned long t = 0);

	protected:
		unsigned long tm_current;   ///< Current frame time in milliseconds.
		unsigned long tm_dt;        ///< Time difference between current and previous frame.
		unsigned long tm_previous;  ///< Previous frame time in milliseconds.

		_Chrono();
	};

	template<class T>
	inline T& _Chrono<T>::Instance()
	{
		static T chrono;
		return chrono;
	}

	template<class T>
	inline _Chrono<T>::_Chrono() : tm_current(0), tm_dt(0), tm_previous(0) { }

	template<class T>
	inline unsigned long _Chrono<T>::current()
	{
		assert(this->tm_current > 0 || !"Rainbow::Chrono: Current time has not been updated.");
		return this->tm_current;
	}

	template<class T>
	inline unsigned long _Chrono<T>::diff()
	{
		return this->tm_dt;
	}

	template<class T>
	inline unsigned long _Chrono<T>::previous()
	{
		assert(this->tm_previous > 0 || !"Rainbow::Chrono: Previous frame time has not been saved.");
		return this->tm_previous;
	}

	template<class T>
	void _Chrono<T>::update(const unsigned long)
	{
		this->tm_previous = this->tm_current;
		this->tm_current = static_cast<T*>(this)->get_time();
		this->tm_dt = this->tm_current - this->tm_previous;
	}
}

#include "Platform.h"
#include "Common/impl/Chrono_Apple.h"
#include "Common/impl/Chrono_SDL.h"
#include "Common/impl/Chrono_Unix.h"
#include "Common/impl/Chrono_Win.h"

#endif
