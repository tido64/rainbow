#ifndef CONFUOCO_RECORDER_H_
#define CONFUOCO_RECORDER_H_

#include "Platform/Definitions.h"
#if defined(RAINBOW_ANDROID)
namespace ConFuoco
{
	struct SLRecorder;
	typedef ConFuoco::SLRecorder* AudioRecorder;
}
#elif defined(RAINBOW_IOS)
@class AVAudioRecorder;
typedef AVAudioRecorder* AudioRecorder;
#else
typedef void* AudioRecorder;
#endif

#include "Common/NonCopyable.h"

namespace ConFuoco
{
	/// Provides audio recording capabilities.
	///
	/// \note Current implementation does not save to a file.
	///
	/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Recorder : private NonCopyable<Recorder>
	{
	public:
		Recorder();
		~Recorder();

		/// Return the average power, in decibels, for the sound being recorded.
		inline float get_average_power() const;

		/// Return low-pass filtered power, in decibels, for the sound being recorded.
		inline float get_low_pass() const;

		/// Return the peak power, in decibels, for the sound being recorded.
		inline float get_peak_power() const;

		/// Pause recording.
		void pause();

		/// Start or resume recording.
		/// \param duration  Maximum duration, in seconds, for the recording.
		/// \return \c true on success; otherwise \c false.
		bool record(const unsigned long duration = 0);

		/// Stop recording and close the audio file.
		void stop();

		/// Refresh the average and peak power values of an audio recorder.
		void update();

		/// \c true if the recorder is successfully initialised; otherwise \c false.
		inline operator bool() const;

	private:
		float average;   ///< Average power in decibels.
		float low_pass;  ///< Low-pass filtered power in decibels.
		float peak;      ///< Peak power in decibels.
		AudioRecorder recorder;
	};

	float Recorder::get_average_power() const
	{
		return this->average;
	}

	float Recorder::get_low_pass() const
	{
		return this->low_pass;
	}

	float Recorder::get_peak_power() const
	{
		return this->peak;
	}

	Recorder::operator bool() const
	{
		return this->recorder;
	}
}

#endif
