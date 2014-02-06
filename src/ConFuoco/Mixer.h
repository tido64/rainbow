// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_MIXER_H_
#define CONFUOCO_MIXER_H_

#include "Common/Vector.h"
#include "ConFuoco/Sound.h"

namespace ConFuoco
{
	/// Audio mixer.
	template<typename T>
	class MixerBase : private NonCopyable<MixerBase<T>>
	{
	public:
		static T *Instance;

		class Channel
		{
			friend T;
			friend MixerBase<T>;

			unsigned int ch;  ///< Channel number.
			Sound *sound;     ///< The sound object bound to this channel.

			Channel() : ch(-1), sound(nullptr) { }
		};

		/// Sets master gain.
		/// \param gain  Scalar amplitude multiplier. The default 1.0 means
		///              that the sound is unattenuated. The value zero equals
		///              silence (no contribution to the output mix).
		void set_gain(const float gain);

		/// Sets global pitch shift.
		/// \param pitch  Desired pitch shift, where 1.0 equals identity. Each
		///               reduction by 50 percent equals a pitch shift of -12
		///               semitones (one octave reduction). Each doubling
		///               equals a pitch shift of 12 semitones (one octave
		///               increase). Zero is not a legal value.
		void set_pitch(const float pitch);

		/// Stops and deletes all sounds.
		void clear();

		/// Suspends/continues processing of audio streams.
		void suspend(const bool);

		/// Updates platform-specific internals.
		void update();

		/* Channel interface. */

		bool is_paused(const Channel *);
		bool is_playing(const Channel *);
		void set_gain(const Channel *, const float gain);
		void pause(const Channel *);
		void play(const Channel *);
		void stop(const Channel *);

		/* Sound interface. */

		/// Creates a sound object from audio file. Deleting a sound object will
		/// automatically stop all channels playing it.
		Sound* create_sound(const char *const file,
		                    const Sound::Type type = Sound::Type::Static,
		                    const int loops = -1);

		/// Starts playback of the sound object and returns the channel on which
		/// it is played. Channels may vary with each playback.
		Channel* play(Sound *);

		/// Releases any resources associated with the sound object.
		void release(Sound *);

	protected:
		static const size_t kNumChannels = 32;

		float master_gain;               ///< Master gain/volume.
		Vector<Sound*> sounds;           ///< Stores all loaded sounds.
		Channel channels[kNumChannels];  ///< Channels on which sounds are played.

		MixerBase();
		~MixerBase();

		/// Finalises initialisation.
		void init();

	private:
		/// Returns the next free channel.
		Channel* next_channel();
	};

	template<typename T>
	T* MixerBase<T>::Instance = nullptr;

	template<typename T>
	MixerBase<T>::MixerBase() : master_gain(0.0f)
	{
		R_ASSERT(!Instance, "An instance of ConFuoco::Mixer already exists");
	}

	template<typename T>
	MixerBase<T>::~MixerBase()
	{
		Instance = nullptr;
	}

	template<typename T>
	void MixerBase<T>::set_gain(const float gain)
	{
		static_cast<T*>(this)->set_gain_impl(gain);
		this->master_gain = gain;
	}

	template<typename T>
	void MixerBase<T>::set_pitch(const float pitch)
	{
		static_cast<T*>(this)->set_pitch_impl(pitch);
	}

	template<typename T>
	void MixerBase<T>::clear()
	{
		static_cast<T*>(this)->clear_impl();
	}

	template<typename T>
	void MixerBase<T>::suspend(const bool suspend)
	{
		static_cast<T*>(this)->suspend_impl(suspend);
	}

	template<typename T>
	void MixerBase<T>::update()
	{
		static_cast<T*>(this)->update_impl();
	}

	/* Channel interface */

	template<typename T>
	bool MixerBase<T>::is_paused(const Channel *c)
	{
		return static_cast<T*>(this)->is_paused_impl(c);
	}

	template<typename T>
	bool MixerBase<T>::is_playing(const Channel *c)
	{
		return static_cast<T*>(this)->is_playing_impl(c);
	}

	template<typename T>
	void MixerBase<T>::set_gain(const Channel *c, const float gain)
	{
		static_cast<T*>(this)->set_gain_impl(c, gain);
	}

	template<typename T>
	void MixerBase<T>::pause(const Channel *c)
	{
		static_cast<T*>(this)->pause_impl(c);
	}

	template<typename T>
	void MixerBase<T>::play(const Channel *c)
	{
		if (!c->sound)
			return;

		static_cast<T*>(this)->play_impl(c);

		R_DEBUG("[Rainbow::ConFuoco] Playing %p (channel %u)\n", static_cast<void*>(c->sound), c->ch);
	}

	template<typename T>
	void MixerBase<T>::stop(const Channel *c)
	{
		static_cast<T*>(this)->stop_impl(c);
	}

	/* Sound interface */

	template<typename T>
	Sound* MixerBase<T>::create_sound(const char *const file,
	                                  const Sound::Type type,
	                                  const int loops)
	{
		Sound *sound = static_cast<T*>(this)->create_sound_impl(file, type, loops);
	#ifndef NDEBUG
		if (!sound)
			R_DEBUG("[Rainbow::ConFuoco] Failed to open '%s'\n", file);
		else
		{
			R_DEBUG("[Rainbow::ConFuoco] Opened '%s' at %p (%s)\n",
			        file, static_cast<void*>(sound),
			        (type == Sound::Type::Stream) ? "stream" : "static");
		}
	#endif
		return sound;
	}

	template<typename T>
	typename MixerBase<T>::Channel* MixerBase<T>::play(Sound *s)
	{
		MixerBase<T>::Channel *channel = this->next_channel();
		if (channel)
		{
			channel->sound = s;
			this->play(channel);
		}
		return channel;
	}

	template<typename T>
	void MixerBase<T>::release(Sound *s)
	{
		static_cast<T*>(this)->release_impl(s);
		this->sounds.qremove(s);

		R_DEBUG("[Rainbow::ConFuoco] Released %p\n", static_cast<void*>(s));
	}

	template<typename T>
	void MixerBase<T>::init()
	{
		this->set_gain(1.0f);
		Instance = static_cast<T*>(this);
	}

	/* Private */

	template<typename T>
	typename MixerBase<T>::Channel* MixerBase<T>::next_channel()
	{
		for (auto &channel : this->channels)
			if (!channel.sound || (!this->is_paused(&channel) && !this->is_playing(&channel)))
				return &channel;
		R_DEBUG("[Rainbow::ConFuoco] All channels are busy\n");
		return nullptr;
	}
}

#if defined(RAINBOW_OS_ANDROID)
#	include "ConFuoco/impl/Mixer_SL.h"
#elif defined(RAINBOW_OS_IOS) || defined(RAINBOW_SDL)
#	include "ConFuoco/impl/Mixer_AL.h"
#else
#	include "ConFuoco/impl/Mixer_Stub.h"
#endif

#endif  // CONFUOCO_MIXER_H_
