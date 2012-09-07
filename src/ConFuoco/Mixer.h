#ifndef CONFUOCO_MIXER_H_
#define CONFUOCO_MIXER_H_

#include "Platform.h"
#if defined(RAINBOW_ANDROID)
#	include "ConFuoco/impl/Engine_SL.h"
#elif defined(RAINBOW_IOS) || defined(RAINBOW_SDL)
#	include "ConFuoco/impl/Engine_AL.h"
#else
#	error "Unknown platform"
#endif

#include "ConFuoco/Sound.h"
#include "ConFuoco/Stream.h"
#include "ConFuoco/WaveBank.h"

namespace ConFuoco
{
	/// Audio mixer.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Mixer : public NonCopyable<Mixer>
	{
	public:
		static inline Mixer& Instance();

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
		inline Wave* load_sfx(const char *const file, const unsigned int instances = 1);

		/// Prepare to stream from file.
		inline Wave* load_stream(const char *const file);

		/// Remove sound/stream and release any associated resources.
		template<class S>
		inline void remove(const S *);

		/// Update all streams.
		inline void update();

	private:
		Engine engine;  ///< Platform-specific audio engine.
		WaveBank bank;  ///< Sound and stream factory.

		inline Mixer();
	};

	Mixer& Mixer::Instance()
	{
		static Mixer mixer;
		return mixer;
	}

	Mixer::Mixer() { }

	void Mixer::clear()
	{
		this->bank.clear();
	}

	bool Mixer::is_available() const
	{
		return this->engine.is_available();
	}

	Wave* Mixer::load_sfx(const char *const file, const unsigned int instances)
	{
		Sound *s = this->bank.create_sound(instances);
		s->load(file);
		return s;
	}

	Wave* Mixer::load_stream(const char *const file)
	{
		Stream *s = this->bank.create_stream();
		s->load(file);
		return s;
	}

	template<class S>
	void Mixer::remove(const S *s)
	{
		this->bank.remove(s);
	}

	void Mixer::set_gain(const float gain) const
	{
		this->engine.set_gain(gain);
	}

	void Mixer::set_orientation(const float r) const
	{
		this->engine.set_orientation(r);
	}

	void Mixer::set_pitch(const float pitch) const
	{
		this->engine.set_pitch(pitch);
	}

	void Mixer::update()
	{
		this->bank.update();
	}
}

#endif
