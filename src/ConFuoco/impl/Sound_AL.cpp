// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/OpenAL.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)

#include <cassert>
#include <cstdio>

#include "ConFuoco/Decoder.h"
#include "ConFuoco/impl/Sound_AL.h"

namespace ConFuoco
{
	Sound::Sound() : bid(0)
	{
		assert(this->sid || !"Rainbow::ConFuoco::AL: Failed to generate source");

		alGenBuffers(1, &this->bid);
		if (alGetError() != AL_NO_ERROR)
		{
			alDeleteSources(1, &this->sid);
			this->sid = 0;
			fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to generate buffers\n");
			return;
		}
		alSourcei(this->sid, AL_BUFFER, this->bid);
	}

	Sound::~Sound()
	{
		if (!this->sid)
			return;

		this->stop();
		alSourcei(this->sid, AL_BUFFER, 0);
		alDeleteBuffers(1, &this->bid);
	}

	bool Sound::load(const char *const file)
	{
		int channels = 0;
		int rate = 0;
		char *data = 0;
		const unsigned int size = Decoder::open_wave(&data, channels, rate, file);

		alBufferData(this->bid, (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, data, size, rate);
		delete[] data;
		return alGetError() == AL_NO_ERROR;
	}
}

#endif
