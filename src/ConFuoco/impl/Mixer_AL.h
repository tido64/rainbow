#ifndef CONFUOCO_MIXER_AL_H_
#define CONFUOCO_MIXER_AL_H_

#include <cmath>

#ifdef RAINBOW_IOS
#	include <AudioToolbox/AudioServices.h>
#endif

#include "Common/Constants.h"
#include "ConFuoco/OpenAL.h"
#include "ConFuoco/WaveBank.h"

namespace ConFuoco
{
	class Wave;

	/// Audio mixer implemented with OpenAL.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Mixer
	{
	public:
		static inline Mixer& Instance();

		~Mixer();

		/// Clear the wave bank.
		inline void clear();

		/// Whether an audio device is available.
		inline bool is_available() const;

		/// Set master gain.
		/// \param gain  Scalar amplitude multiplier. The default 1.0 means
		///              that the sound is unattenuated. The value zero equals
		///              silence (no contribution to the output mix).
		inline void set_gain(const float gain) const;

		inline void set_orientation(const float r) const;

		/// Set global pitch shift.
		/// \param pitch  Desired pitch shift, where 1.0 equals identity. Each
		///               reduction by 50 percent equals a pitch shift of -12
		///               semitones (one octave reduction). Each doubling
		///               equals a pitch shift of 12 semitones (one octave
		///               increase). Zero is not a legal value.
		inline void set_pitch(const float pitch) const;

		/// Load audio file into buffer.
		Wave* load_sfx(const char *const file);

		/// Prepare to stream from file.
		Wave* load_stream(const char *const file);

		/// Remove sound/stream and release any resources associated.
		template<class T>
		inline void remove(const T *);

		/// Update all streams.
		inline void update();

	private:
	#ifdef RAINBOW_IOS
		static void InterruptionListener(void*, UInt32);
		static void RouteChangeListener(void*, AudioSessionPropertyID, UInt32, const void*);
	#endif

		ALCcontext *context;  ///< Active OpenAL context.
		WaveBank bank;        ///< Sound and stream factory.

		Mixer();
		Mixer(const Mixer &);
		Mixer& operator=(const Mixer &);
	};

	Mixer& Mixer::Instance()
	{
		static Mixer mixer;
		return mixer;
	}

	void Mixer::clear()
	{
		this->bank.clear();
	}

	bool Mixer::is_available() const
	{
		return this->context;
	}

	template<class T>
	void Mixer::remove(const T *s)
	{
		this->bank.remove(s);
	}

	void Mixer::set_gain(const float gain) const
	{
		alListenerf(AL_GAIN, gain);
	}

	void Mixer::set_orientation(const float r) const
	{
		float ori[] = { cosf(r + kPi_2), sinf(r + kPi_2), 0.0f, 0.0f, 0.0f, 1.0f };
		alListenerfv(AL_ORIENTATION, ori);
	}

	void Mixer::set_pitch(const float pitch) const
	{
		alListenerf(AL_PITCH, (pitch < 0.01f) ? 0.01f : pitch);
	}

	void Mixer::update()
	{
		this->bank.update();
	}
}

#endif
