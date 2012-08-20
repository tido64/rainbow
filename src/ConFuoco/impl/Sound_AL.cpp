// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/OpenAL.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)
#	include "Common/Debug.h"
#	include "ConFuoco/Decoder.h"
#	include "ConFuoco/impl/Sound_AL.h"

namespace ConFuoco
{
	Sound::Sound(const unsigned int i) :
		paused(false), curr(0), bid(0), sid_count(i), sids(nullptr)
	{
		alGenBuffers(1, &this->bid);
		if (alGetError() != AL_NO_ERROR)
		{
			R_ERROR("[Rainbow::ConFuoco] AL: Failed to generate buffer\n");
			return;
		}

		this->sids = new unsigned int[this->sid_count];
		alGenSources(this->sid_count, this->sids);
		if (alGetError() != AL_NO_ERROR)
		{
			delete[] this->sids;
			this->sids = nullptr;
			alDeleteBuffers(1, &this->bid);

			R_ERROR("[Rainbow::ConFuoco] AL: Failed to generate sources\n");
			return;
		}

		for (unsigned int i = 0; i < this->sid_count; ++i)
		{
			alSourcei(this->sids[i], AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef(this->sids[i], AL_ROLLOFF_FACTOR, 0.0f);
		}
	}

	Sound::~Sound()
	{
		if (!this->sids)
			return;

		this->stop();

		for (unsigned int i = 0; i < this->sid_count; ++i)
			alSourcei(this->sids[i], AL_BUFFER, 0);
		alDeleteSources(this->sid_count, this->sids);
		delete[] this->sids;

		alDeleteBuffers(1, &this->bid);
	}

	bool Sound::load(const char *const file)
	{
		int channels = 0;
		int rate = 0;
		char *data = nullptr;
		const size_t size = Decoder::open_wave(&data, channels, rate, file);

		for (unsigned int i = 0; i < this->sid_count; ++i)
			alSourcei(this->sids[i], AL_BUFFER, 0);

		alBufferData(this->bid, (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, size, rate);
		delete[] data;

		for (unsigned int i = 0; i < this->sid_count; ++i)
			alSourcei(this->sids[i], AL_BUFFER, this->bid);

		return alGetError() == AL_NO_ERROR;
	}

	void Sound::set_gain(const float gain)
	{
		for (unsigned int i = 0; i < this->sid_count; ++i)
			alSourcef(this->sids[i], AL_GAIN, gain);
	}

	void Sound::set_loops(const int) { }

	void Sound::pause()
	{
		int state = 0;
		for (unsigned int i = 0; i < this->sid_count; ++i)
		{
			alGetSourcei(this->sids[i], AL_SOURCE_STATE, &state);
			if (state != AL_PLAYING)
				continue;

			alSourcePause(this->sids[i]);
			this->paused = true;
		}
	}

	void Sound::play(const float x, const float y, const float z)
	{
		int state = 0;
		if (this->paused)
		{
			for (unsigned int i = 0; i < this->sid_count; ++i)
			{
				alGetSourcei(this->sids[i], AL_SOURCE_STATE, &state);
				if (state != AL_PAUSED)
					continue;

				alSourcePlay(this->sids[i]);
			}
			this->paused = false;
		}
		else
		{
			alGetSourcei(this->sids[this->curr], AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
			{
				++this->curr %= this->sid_count;
				alSourceStop(this->sids[this->curr]);
			}

			alSource3f(this->sids[this->curr], AL_POSITION, x, y, z);
			alSourcePlay(this->sids[this->curr]);
		}
	}

	void Sound::stop()
	{
		for (unsigned int i = 0; i < this->sid_count; ++i)
			alSourceStop(this->sids[i]);
		this->paused = false;
	}
}

#endif
