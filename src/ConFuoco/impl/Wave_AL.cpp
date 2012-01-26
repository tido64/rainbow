// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/OpenAL.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)

#include <cstdio>

#include "ConFuoco/impl/Wave_AL.h"

namespace ConFuoco
{
	namespace AL
	{
		Wave::Wave() : sid(0)
		{
			alGenSources(1, &this->sid);
			if (alGetError() != AL_NO_ERROR)
			{
				fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to generate source\n");
				return;
			}
			alSourcei(this->sid, AL_SOURCE_RELATIVE, AL_TRUE);
			alSourcef(this->sid, AL_ROLLOFF_FACTOR, 0.0f);
		}

		Wave::~Wave()
		{
			if (!this->sid)
				return;

			alDeleteSources(1, &this->sid);
		}

		void Wave::set_gain(const float gain)
		{
			alSourcef(this->sid, AL_GAIN, gain);
		}

		void Wave::set_loops(const int) { }

		void Wave::pause() const
		{
			int state = 0;
			alGetSourcei(this->sid, AL_SOURCE_STATE, &state);
			if (state == AL_STOPPED)
				return;

			alSourcePause(this->sid);
		}

		void Wave::play(const float x, const float y, const float z) const
		{
			int state = 0;
			alGetSourcei(this->sid, AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
				return;

			alSource3f(this->sid, AL_POSITION, x, y, z);
			alSourcePlay(this->sid);
		}

		void Wave::stop() const
		{
			alSourceStop(this->sid);
		}
	}
}

#endif
