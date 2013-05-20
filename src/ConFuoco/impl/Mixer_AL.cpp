/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#if !defined(RAINBOW_JS) && (defined(RAINBOW_IOS) || defined(RAINBOW_SDL))

#if defined(RAINBOW_IOS) || defined(RAINBOW_MAC)
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#include "Common/Debug.h"
#include "ConFuoco/AudioFile.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/Sound.h"
#include "ConFuoco/impl/Mixer_iOS.h"

namespace ConFuoco
{
	Mixer* Mixer::Instance = nullptr;

	namespace
	{
		const size_t kALNumBuffers = 3;

		struct Wave : public Sound
		{
			unsigned int bid;

			Wave(Mixer *m, const char *const file) : Sound(STATIC, m), bid(0)
			{
				alGenBuffers(1, &this->bid);
				if (alGetError() != AL_NO_ERROR)
					R_ERROR("[Rainbow::ConFuoco/AL] Failed to generate buffer\n");

				AudioFile *audio_file = AudioFile::Open(file, AudioFile::kAudioFileStatic);
				char *data;
				const size_t size = audio_file->read(&data);
				alBufferData(
					this->bid,
					(audio_file->get_channels() == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
					data,
					size,
					audio_file->get_rate());
				delete[] data;
				delete audio_file;
			}

			virtual ~Wave()
			{
				this->mixer->release(this);
				alDeleteBuffers(1, &this->bid);
			}
		};

	#ifndef RAINBOW_IOS
		struct Stream;
		Vector<Stream*> streams;

		struct Stream : public Sound
		{
			bool playing;
			int format;
			int rate;
			int loops;

			size_t buf_sz;
			char *buffer;
			AudioFile *audio_file;

			Channel *channel;
			unsigned int bids[kALNumBuffers];

			Stream(Mixer *m, const char *const file, const int loops) :
				Sound(STREAM, m), playing(false), format(0), rate(0),
				loops(loops), buf_sz(0), buffer(nullptr), audio_file(nullptr),
				channel(nullptr)
			{
				alGenBuffers(kALNumBuffers, this->bids);
				if (alGetError() != AL_NO_ERROR)
					R_ERROR("[Rainbow::ConFuoco/AL] Failed to generate buffers\n");

				this->audio_file = AudioFile::Open(file, AudioFile::kAudioFileStream);
				int channels = this->audio_file->get_channels();
				this->buf_sz = AudioFile::CreateBuffer(&this->buffer, channels);
				this->format = (channels == 1)
				             ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
				this->rate = this->audio_file->get_rate();

				this->preload();
				streams.push_back(this);
			}

			virtual ~Stream()
			{
				streams.qremove(this);
				this->mixer->release(this);
				alDeleteBuffers(kALNumBuffers, this->bids);
				delete this->audio_file;
				delete[] this->buffer;
			}

			void preload()
			{
				this->rewind();
				for (size_t i = 0; i < kALNumBuffers; ++i)
				{
					const size_t size = this->audio_file->read(this->buffer, this->buf_sz);
					if (!size)
						break;
					alBufferData(this->bids[i], this->format, this->buffer, size, this->rate);
				}
			}

			size_t read()
			{
				return this->audio_file->read(this->buffer, this->buf_sz);
			}

			void rewind()
			{
				this->audio_file->rewind();
			}
		};
	#endif
	}

	Mixer::Mixer() : master_gain(1.0f), context(nullptr), sounds(num_channels)
	{
		R_ASSERT(!Instance, "An instance of ConFuoco::Mixer already exists");

	#ifdef RAINBOW_IOS
		InitAudioSession(this);
	#endif

		ALCdevice *device = alcOpenDevice(0);
		if (!device)
		{
			R_ERROR("[Rainbow::ConFuoco/AL] alcOpenDevice error code 0x%x\n", alGetError());
			return;
		}

		ALCcontext *ctx = alcCreateContext(device, 0);
		if (!ctx)
		{
			R_ERROR("[Rainbow::ConFuoco/AL] alcCreateContext error code 0x%x\n", alGetError());
			return;
		}
		alcMakeContextCurrent(ctx);
		this->context = ctx;

		// Mac: Clear any errors from previous sessions.
		alGetError();

		unsigned int sids[num_channels];
		alGenSources(num_channels, sids);
		if (alGetError() != AL_NO_ERROR)
		{
			R_ERROR("[Rainbow::ConFuoco/AL] Failed to generate sources\n");
			return;
		}
		for (size_t i = 0; i < num_channels; ++i)
		{
			alSourcei(sids[i], AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef(sids[i], AL_ROLLOFF_FACTOR, 0.0f);
			this->channels[i].ch = sids[i];
			this->channels[i].mixer = this;
		}

		Instance = this;
	}

	Mixer::~Mixer()
	{
		Instance = nullptr;

		this->clear();
		for (size_t i = 0; i < num_channels; ++i)
			alDeleteSources(1, &this->channels[i].ch);

		ALCdevice *device = alcGetContextsDevice(static_cast<ALCcontext*>(this->context));
		this->suspend(true);
		alcDestroyContext(static_cast<ALCcontext*>(this->context));
		alcCloseDevice(device);
	}

	void Mixer::set_gain(const float gain)
	{
		alListenerf(AL_GAIN, gain);
		this->master_gain = gain;
	}

	void Mixer::set_pitch(const float pitch)
	{
		alListenerf(AL_PITCH, (pitch < 0.01f) ? 0.01f : pitch);
	}

	void Mixer::clear()
	{
	#ifndef RAINBOW_IOS
		streams.clear();
	#endif

		size_t i = this->sounds.size();
		while (i)
			delete this->sounds[--i];

		R_ASSERT(this->sounds.size() == 0, "Failed to clear sound bank");
	}

	Sound* Mixer::create_sound(const char *const file, const int type, const int loops)
	{
		Sound *snd = nullptr;
		if (type != STATIC)
		{
			snd = new Stream(this, file, loops);
			R_DEBUG("[Rainbow::ConFuoco/AL] '%s' => %p (stream)\n", file, static_cast<void*>(snd));
		}
		else
		{
			snd = new Wave(this, file);
			R_DEBUG("[Rainbow::ConFuoco/AL] '%s' => %p (static)\n", file, static_cast<void*>(snd));
		}
		this->sounds.push_back(snd);
		return snd;
	}

	Channel* Mixer::play(Sound *snd)
	{
		if (!snd)
			return nullptr;

		Channel *channel = nullptr;
		if (snd->type != STATIC)
		{
			Stream *stream = static_cast<Stream*>(snd);
			if (stream->channel && stream->channel->sound == stream)
			{
				channel = stream->channel;
				this->stop(channel);
			}
			else
			{
				channel = this->next_channel();
				if (!channel)
					return nullptr;

				channel->sound = snd;
				stream->channel = channel;
			}
			this->set_gain(channel, 1.0f);
		#ifdef RAINBOW_IOS
			stream->play();
		#else
			alSourcei(channel->ch, AL_BUFFER, 0);
			alSourceQueueBuffers(channel->ch, kALNumBuffers, stream->bids);
			this->play(channel);
			stream->playing = true;
		#endif
		}
		else
		{
			channel = this->next_channel();
			if (!channel)
				return nullptr;

			alSourcei(channel->ch, AL_BUFFER, static_cast<Wave*>(snd)->bid);
			channel->sound = snd;
			this->set_gain(channel, 1.0f);
			this->play(channel);
		}
		R_DEBUG("[Rainbow::ConFuoco/AL] %p -> %u\n", static_cast<void*>(snd), channel->ch);
		return channel;
	}

	void Mixer::release(Sound *snd)
	{
		Channel *ch = this->channels;
		for (size_t i = 0; i < num_channels; ++i, ++ch)
		{
			if (ch->sound == snd)
			{
				this->stop(ch);
				alSourcei(ch->ch, AL_BUFFER, 0);
				ch->sound = nullptr;
			}
		}
		this->sounds.qremove(snd);
		R_DEBUG("[Rainbow::ConFuoco/AL] <- %p\n", static_cast<void*>(snd));
	}

	void Mixer::suspend(const bool suspend)
	{
		ALCcontext *context = static_cast<ALCcontext*>(this->context);
		if (suspend)
		{
			alcSuspendContext(context);
			alcMakeContextCurrent(nullptr);
		}
		else
		{
			alcMakeContextCurrent(context);
			alcProcessContext(context);
		}
	}

	void Mixer::update()
	{
	#ifndef RAINBOW_IOS
		for (size_t i = 0; i < streams.size(); ++i)
		{
			Stream *s = streams[i];
			if (!s->playing)
				continue;

			int processed = 0;
			alGetSourcei(s->channel->ch, AL_BUFFERS_PROCESSED, &processed);
			for (int i = 0; i < processed; ++i)
			{
				if (!s->read())
				{
					if (!s->loops)
					{
						processed = i;
						s->playing = false;
						break;
					}
					else if (s->loops > 0)
						--s->loops;
					s->rewind();
					s->read();
				}
				unsigned int buffer = 0;
				alSourceUnqueueBuffers(s->channel->ch, 1, &buffer);
				alBufferData(buffer, s->format, s->buffer, s->buf_sz, s->rate);
				alSourceQueueBuffers(s->channel->ch, 1, &buffer);
			}
			if (processed > 0 && !this->is_playing(s->channel))
				this->play(s->channel);
		}
	#endif
	}

	bool Mixer::is_paused(Channel *ch)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
			return static_cast<Stream*>(ch->sound)->paused;
	#endif

		int state = 0;
		alGetSourcei(ch->ch, AL_SOURCE_STATE, &state);
		return state == AL_PAUSED;
	}

	bool Mixer::is_playing(Channel *ch)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			Stream *s = static_cast<Stream*>(ch->sound);
			return s->paused || s->player.playing;
		}
	#endif

		int state = 0;
		alGetSourcei(ch->ch, AL_SOURCE_STATE, &state);
		return state == AL_PAUSED || state == AL_PLAYING;
	}

	void Mixer::set_gain(Channel *ch, const float gain)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			static_cast<Stream*>(ch->sound)->player.volume = gain;
			return;
		}
	#endif

		alSourcef(ch->ch, AL_GAIN, gain);
	}

	void Mixer::pause(Channel *ch)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			static_cast<Stream*>(ch->sound)->pause();
			return;
		}
	#endif

		alSourcePause(ch->ch);
	}

	void Mixer::play(Channel *ch)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			static_cast<Stream*>(ch->sound)->play();
			return;
		}
	#endif

		alSourcePlay(ch->ch);
	}

	void Mixer::stop(Channel *ch)
	{
	#ifdef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			static_cast<Stream*>(ch->sound)->stop();
			return;
		}
	#endif

		alSourceStop(ch->ch);

	#ifndef RAINBOW_IOS
		if (ch->sound->type != STATIC)
		{
			Stream *s = static_cast<Stream*>(ch->sound);
			s->playing = false;
			alSourcei(ch->ch, AL_BUFFER, 0);
			s->preload();
		}
	#endif
	}
}

#endif
