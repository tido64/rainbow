#ifndef CHRONO_H_
#define CHRONO_H_

#include "Common/RainbowAssert.h"

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
		static inline T& Instance();

		virtual ~_Chrono() { }

		/// Return timestamp of current frame.
		inline unsigned long current();

		/// Return the time difference between current and previous frame.
		inline unsigned long diff();

		/// Return timestamp of previous frame.
		inline unsigned long previous();

		/// Update the clock using an external or internal source, depending on
		/// the platform.
		void update(const unsigned long t = 0);

	protected:
		unsigned long tm_current;   ///< Current frame time in milliseconds.
		unsigned long tm_dt;        ///< Time difference between current and previous frame.
		unsigned long tm_previous;  ///< Previous frame time in milliseconds.

		inline _Chrono();
	};

	template<class T>
	T& _Chrono<T>::Instance()
	{
		static T chrono;
		return chrono;
	}

	template<class T>
	_Chrono<T>::_Chrono() : tm_current(0), tm_dt(0), tm_previous(0) { }

	template<class T>
	unsigned long _Chrono<T>::current()
	{
		R_ASSERT(this->tm_current > 0, "Current time has not been updated");
		return this->tm_current;
	}

	template<class T>
	unsigned long _Chrono<T>::diff()
	{
		return this->tm_dt;
	}

	template<class T>
	unsigned long _Chrono<T>::previous()
	{
		R_ASSERT(this->tm_previous > 0, "Previous frame time has not been saved");
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
#include "Common/impl/Chrono_Android.h"
#include "Common/impl/Chrono_Apple.h"
#include "Common/impl/Chrono_SDL.h"

#endif
