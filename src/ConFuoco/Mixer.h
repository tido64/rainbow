#ifndef CONFUOCO_MIXER_H_
#define CONFUOCO_MIXER_H_

#include "Common/Vector.h"
#include "ConFuoco/Channel.h"

namespace ConFuoco
{
	class Sound;

	enum Type
	{
		STATIC,
		STREAM
	};

	/// Audio mixer.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Mixer : public NonCopyable<Mixer>
	{
		friend class Channel;

	public:
		static const size_t num_channels = 32;
		static Mixer* Instance;

		Mixer();
		~Mixer();

		/// Set master gain.
		/// \param gain  Scalar amplitude multiplier. The default 1.0 means
		///              that the sound is unattenuated. The value zero equals
		///              silence (no contribution to the output mix).
		void set_gain(const float gain);

		/// Set global pitch shift.
		/// \param pitch  Desired pitch shift, where 1.0 equals identity. Each
		///               reduction by 50 percent equals a pitch shift of -12
		///               semitones (one octave reduction). Each doubling
		///               equals a pitch shift of 12 semitones (one octave
		///               increase). Zero is not a legal value.
		void set_pitch(const float pitch);

		/// Stop and delete all sounds.
		void clear();

		/// Create a sound object from audio file. Deleting a sound object will
		/// automatically stop all channels from playing it.
		Sound* create_sound(const char *const file, const int type = STATIC, const int loops = -1);

		/// Start playback of the sound object and return the channel on which
		/// it is played. Channels may vary with each playback.
		Channel* play(Sound *);

		/// Release any resources associated with the sound object.
		void release(Sound *);

		/// Suspend/continue processing of audio streams.
		void suspend(const bool);

		/// Update platform-specific internals.
		void update();

	private:
		float master_gain;               ///< Master gain/volume.
		void *context;                   ///< Platform-specific audio context.
		Vector<Sound*> sounds;           ///< Stores all loaded sounds.
		Channel channels[num_channels];  ///< Channels on which sounds are played.

		/// Return the next free channel.
		inline Channel* next_channel();

		/* Implement Channel interface. */

		bool is_paused(Channel *);
		bool is_playing(Channel *);
		void set_gain(Channel *, const float gain);
		void pause(Channel *);
		void play(Channel *);
		void stop(Channel *);
	};

	Channel* Mixer::next_channel()
	{
		for (size_t i = 0; i < num_channels; ++i)
			if (!this->channels[i].sound || !this->channels[i].is_playing())
				return this->channels + i;
		return nullptr;
	}
}

#endif
