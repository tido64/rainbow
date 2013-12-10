/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || (defined(RAINBOW_SDL) && !defined(RAINBOW_JS))

#include <memory>

#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
#	include <OpenAL/al.h>
#	include <OpenAL/alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#include "ConFuoco/AudioFile.h"
#include "ConFuoco/Mixer.h"
#ifdef RAINBOW_OS_IOS
#	include "ConFuoco/impl/Mixer_iOS.h"
#endif

#define CF_TAG "[Rainbow::ConFuoco/AL] "

namespace
{
	int alGetSourceState(const unsigned int sid)
	{
		int value = 0;
		alGetSourcei(sid, AL_SOURCE_STATE, &value);
		return value;
	}

	struct Wave : public ConFuoco::Sound
	{
		unsigned int bid;

		Wave(const char *const file);
		virtual ~Wave();
	};

#ifndef RAINBOW_OS_IOS

	const size_t kAudioBufferSize = 4096;
	const size_t kNumALBuffers = 3;

	char g_audio_buffer[kAudioBufferSize];

	struct Stream : public ConFuoco::Sound
	{
		bool playing;
		int format;
		int rate;
		int loops;

		const ConFuoco::Mixer::Channel *channel;
		std::unique_ptr<ConFuoco::AudioFile> audio_file;

		unsigned int bids[kNumALBuffers];

		Stream(const char *const file, const int loops);
		virtual ~Stream();

		void preload();
		size_t read();
		void rewind();
	};

#endif
}

namespace ConFuoco
{
	MixerAL::MixerAL() : context_(nullptr)
	{
	#ifdef RAINBOW_OS_IOS
		this->session_ = [[AudioSession alloc] init];
	#endif

		ALCdevice *device = alcOpenDevice(0);
		if (!device)
		{
			R_ERROR(CF_TAG "alcOpenDevice error code 0x%x\n", alGetError());
			return;
		}

		ALCcontext *context = alcCreateContext(device, 0);
		if (!context)
		{
			R_ERROR(CF_TAG "alcCreateContext error code 0x%x\n", alGetError());
			return;
		}
		alcMakeContextCurrent(context);

		// Mac: Clear any errors from previous sessions.
		alGetError();

		unsigned int sids[kNumChannels];
		alGenSources(kNumChannels, sids);
		if (alGetError() != AL_NO_ERROR)
		{
			R_ERROR(CF_TAG "Failed to generate sources\n");
			alcDestroyContext(context);
			return;
		}
		for (size_t i = 0; i < kNumChannels; ++i)
		{
			alSourcei(sids[i], AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef(sids[i], AL_ROLLOFF_FACTOR, 0.0f);
			this->channels[i].ch = sids[i];
		}
		this->context_ = context;
		this->init();
	}

	MixerAL::~MixerAL()
	{
		if (!this->context_)
			return;

		this->clear();
		for (auto &channel : this->channels)
			alDeleteSources(1, &channel.ch);

		ALCdevice *device = alcGetContextsDevice(this->context_);
		this->suspend(true);
		alcDestroyContext(this->context_);
		alcCloseDevice(device);
	}

	void MixerAL::set_gain_impl(const float gain)
	{
		alListenerf(AL_GAIN, gain);
	}

	void MixerAL::set_pitch_impl(const float pitch)
	{
		alListenerf(AL_PITCH, (pitch < 0.01f) ? 0.01f : pitch);
	}

	void MixerAL::clear_impl()
	{
		size_t i = this->sounds.size();
		while (i)
			delete this->sounds[--i];

		R_ASSERT(this->sounds.size() == 0, "Failed to clear sound bank");
	}

	void MixerAL::suspend_impl(const bool suspend)
	{
		if (suspend)
		{
			alcSuspendContext(this->context_);
			alcMakeContextCurrent(nullptr);
		}
		else
		{
			alcMakeContextCurrent(this->context_);
			alcProcessContext(this->context_);
		}
	}

	void MixerAL::update_impl()
	{
	#ifndef RAINBOW_OS_IOS
		for (auto sound : this->sounds)
		{
			if (sound->type != Sound::Type::Stream)
				continue;

			Stream *stream = static_cast<Stream*>(sound);
			if (!stream->playing)
				continue;

			int processed = 0;
			alGetSourcei(stream->channel->ch, AL_BUFFERS_PROCESSED, &processed);
			for (int i = 0; i < processed; ++i)
			{
				size_t read = stream->read();
				if (read == 0)
				{
					if (!stream->loops)
					{
						processed = i;
						stream->playing = false;
						break;
					}
					else if (stream->loops > 0)
						--stream->loops;
					stream->rewind();
					read = stream->read();
				}
				unsigned int buffer = 0;
				alSourceUnqueueBuffers(stream->channel->ch, 1, &buffer);
				alBufferData(buffer, stream->format, g_audio_buffer, read, stream->rate);
				alSourceQueueBuffers(stream->channel->ch, 1, &buffer);
			}
			if (processed > 0 && alGetSourceState(stream->channel->ch) == AL_STOPPED)
				alSourcePlay(stream->channel->ch);
		}
	#endif
	}

	/* Channel interface */

	bool MixerAL::is_paused_impl(const Channel *c)
	{
	#ifdef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
			return static_cast<Stream*>(c->sound)->is_paused();
	#endif
		return alGetSourceState(c->ch) == AL_PAUSED;
	}

	bool MixerAL::is_playing_impl(const Channel *c)
	{
	#ifdef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
			return static_cast<Stream*>(c->sound)->is_playing();
	#endif
		return alGetSourceState(c->ch) == AL_PLAYING;
	}

	void MixerAL::set_gain_impl(const Channel *c, const float gain)
	{
	#ifdef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
		{
			static_cast<Stream*>(c->sound)->set_volume(gain);
			return;
		}
	#endif
		alSourcef(c->ch, AL_GAIN, gain);
	}

	void MixerAL::pause_impl(const Channel *c)
	{
	#ifdef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
		{
			Stream *sound = static_cast<Stream*>(c->sound);
			if (sound->is_paused())
				sound->play();
			else if (sound->is_playing())
				sound->pause();
			return;
		}
	#endif

		const int state = alGetSourceState(c->ch);
		if (state == AL_STOPPED)
			return;

		if (state == AL_PAUSED)
			alSourcePlay(c->ch);
		else
			alSourcePause(c->ch);
	}

	void MixerAL::play_impl(const Channel *c)
	{
		if (c->sound->type != Sound::Type::Static)
		{
			Stream *stream = static_cast<Stream*>(c->sound);
			if (stream->channel && stream->channel->sound == stream)
				this->stop(stream->channel);
			stream->channel = c;
			this->set_gain(c, 1.0f);
		#ifdef RAINBOW_OS_IOS
			stream->play();
		#else
			alSourcei(c->ch, AL_BUFFER, 0);
			alSourceQueueBuffers(c->ch, kNumALBuffers, stream->bids);
			alSourcePlay(c->ch);
			stream->playing = true;
		#endif
		}
		else
		{
			alSourcei(c->ch, AL_BUFFER, static_cast<Wave*>(c->sound)->bid);
			this->set_gain(c, 1.0f);
			alSourcePlay(c->ch);
		}
	}

	void MixerAL::stop_impl(const Channel *c)
	{
	#ifdef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
		{
			static_cast<Stream*>(c->sound)->stop();
			return;
		}
	#endif
		alSourceStop(c->ch);
	#ifndef RAINBOW_OS_IOS
		if (c->sound->type != Sound::Type::Static)
		{
			alSourcei(c->ch, AL_BUFFER, 0);
			Stream *s = static_cast<Stream*>(c->sound);
			s->playing = false;
			s->preload();
		}
	#endif
	}

	/* Sound interface */

	Sound* MixerAL::create_sound_impl(const char *const file,
	                                  const Sound::Type type,
	                                  const int loops)
	{
		Sound *sound = nullptr;
		if (type != Sound::Type::Static)
			sound = new Stream(file, loops);
		else
			sound = new Wave(file);
		this->sounds.push_back(sound);
		return sound;
	}

	void MixerAL::release_impl(Sound *s)
	{
		for (auto &channel : this->channels)
		{
			if (channel.sound == s)
			{
				this->stop(&channel);
				channel.sound = nullptr;
				alSourcei(channel.ch, AL_BUFFER, 0);
			}
		}
	}
}

Wave::Wave(const char *const file) :
	ConFuoco::Sound(ConFuoco::Sound::Type::Static), bid(0)
{
	alGenBuffers(1, &this->bid);
	if (alGetError() != AL_NO_ERROR)
		R_ERROR(CF_TAG "Failed to generate buffer\n");

	std::unique_ptr<ConFuoco::AudioFile> audio_file(
			ConFuoco::AudioFile::Open(file, ConFuoco::AudioFile::kAudioFileStatic));
	char *data;
	const size_t size = audio_file->read(&data);
	alBufferData(this->bid,
	             (audio_file->channels() == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
	             data,
	             size,
	             audio_file->rate());
	delete[] data;
}

Wave::~Wave()
{
	ConFuoco::Mixer::Instance->release(this);
	alDeleteBuffers(1, &this->bid);
}

#ifndef RAINBOW_OS_IOS

Stream::Stream(const char *const file, const int loops) :
	ConFuoco::Sound(ConFuoco::Sound::Type::Stream), playing(false), format(0),
	rate(0), loops(loops), channel(nullptr)
{
	alGenBuffers(kNumALBuffers, this->bids);
	if (alGetError() != AL_NO_ERROR)
		R_ERROR(CF_TAG "Failed to generate buffers\n");

	this->audio_file.reset(
			ConFuoco::AudioFile::Open(file, ConFuoco::AudioFile::kAudioFileStream));
	this->format = (this->audio_file->channels() == 1)
	             ? AL_FORMAT_MONO16
	             : AL_FORMAT_STEREO16;
	this->rate = this->audio_file->rate();

	this->preload();
}

Stream::~Stream()
{
	ConFuoco::Mixer::Instance->release(this);
	alDeleteBuffers(kNumALBuffers, this->bids);
}

void Stream::preload()
{
	this->rewind();
	for (size_t i = 0; i < kNumALBuffers; ++i)
	{
		if (!this->audio_file->read(g_audio_buffer, kAudioBufferSize))
			break;
		alBufferData(this->bids[i], this->format, g_audio_buffer, kAudioBufferSize, this->rate);
	}
}

size_t Stream::read()
{
	return this->audio_file->read(g_audio_buffer, kAudioBufferSize);
}

void Stream::rewind()
{
	this->audio_file->rewind();
}

#endif  // !RAINBOW_OS_IOS
#endif  // RAINBOW_OS_IOS || (RAINBOW_SDL && !RAINBOW_JS)
