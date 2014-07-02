// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_RECORDER_H_
#define CONFUOCO_RECORDER_H_

#include "Common/Algorithm.h"
#include "Common/NonCopyable.h"

namespace ConFuoco
{
	/// Provides audio recording capabilities.
	///
	/// \note Current implementation does not save to a file.
	template<typename T>
	class RecorderBase : private NonCopyable<RecorderBase<T>>
	{
	public:
		/// Returns the average power, in decibels, for the sound being recorded.
		float average_power() const;

		/// Returns low-pass filtered power, in decibels, for the sound being recorded.
		float low_pass() const;

		/// Returns the peak power, in decibels, for the sound being recorded.
		float peak_power() const;

		/// Pauses recording.
		void pause();

		/// Starts or resumes recording.
		/// \param duration  Maximum duration, in seconds, for the recording.
		/// \return \c true on success; otherwise \c false.
		bool record(const unsigned long duration = 0);

		/// Stops recording and closes the audio file.
		void stop();

		/// Refreshes the average and peak power values of the recording.
		void update();

	protected:
		float average;  ///< Average power in decibels.
		float low;      ///< Low-pass filtered power in decibels.
		float peak;     ///< Peak power in decibels.

		RecorderBase();
		~RecorderBase();
	};

	template<typename T>
	float RecorderBase<T>::average_power() const
	{
		return this->average;
	}

	template<typename T>
	float RecorderBase<T>::low_pass() const
	{
		return this->low;
	}

	template<typename T>
	float RecorderBase<T>::peak_power() const
	{
		return this->peak;
	}

	template<typename T>
	void RecorderBase<T>::pause()
	{
		static_cast<T*>(this)->pause_impl();
	}

	template<typename T>
	bool RecorderBase<T>::record(const unsigned long duration)
	{
		return static_cast<T*>(this)->record_impl(duration);
	}

	template<typename T>
	void RecorderBase<T>::stop()
	{
		static_cast<T*>(this)->stop_impl();
	}

	template<typename T>
	void RecorderBase<T>::update()
	{
		static_cast<T*>(this)->update_impl();
		this->low = Rainbow::low_pass(this->peak, this->low);
	}

	template<typename T>
	RecorderBase<T>::RecorderBase() : average(-160.0f), low(-160.0f), peak(-160.0f) { }

	template<typename T>
	RecorderBase<T>::~RecorderBase() = default;
}

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_ANDROID) && !defined(USE_FMOD_STUDIO)
#	include "ConFuoco/impl/Recorder_SL.h"
#elif defined(RAINBOW_OS_IOS) && !defined(USE_FMOD_STUDIO)
#	include "ConFuoco/impl/Recorder_iOS.h"
#else

namespace ConFuoco
{
	class RecorderStub : public RecorderBase<RecorderStub>
	{
		friend RecorderBase<RecorderStub>;

		void pause_impl() { }
		bool record_impl(const unsigned long) { return false; }
		void stop_impl() { }
		void update_impl() { }
	};

	typedef RecorderStub Recorder;
}

#endif

#endif
