// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/OpenAL.h"
#ifdef RAINBOW_SDL

#include <cstdio>
#include <cstring>

#include "Common/RainbowAssert.h"
#include "ConFuoco/Decoder.h"
#include "ConFuoco/impl/Stream_AL.h"

namespace ConFuoco
{
	Stream::Stream() :
		playing(false), sid(0), format(0), rate(0), loops(-1), buffer_size(0),
		buffer(nullptr), handle(nullptr)
	{
		alGenBuffers(CONFUOCO_STREAM_AL_BUFFERS, this->bid);
		if (alGetError() != AL_NO_ERROR)
		{
			fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to generate buffers\n");
			return;
		}

		alGenSources(1, &this->sid);
		if (alGetError() != AL_NO_ERROR)
		{
			alDeleteBuffers(CONFUOCO_STREAM_AL_BUFFERS, this->bid);
			memset(this->bid, 0, CONFUOCO_STREAM_AL_BUFFERS * sizeof(*this->bid));

			fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to generate source\n");
			return;
		}

		alSourcei(this->sid, AL_SOURCE_RELATIVE, AL_TRUE);
		alSourcef(this->sid, AL_ROLLOFF_FACTOR, 0.0f);
	}

	Stream::~Stream()
	{
		if (!this->sid)
			return;

		if (this->handle)
			this->release();

		alDeleteSources(1, &this->sid);
		alDeleteBuffers(CONFUOCO_STREAM_AL_BUFFERS, this->bid);
	}

	void Stream::update()
	{
		if (!this->playing)
			return;

		int processed = 0;
		alGetSourcei(this->sid, AL_BUFFERS_PROCESSED, &processed);
		for (int i = 0; i < processed; ++i)
		{
			if (!Decoder::read(this->buffer, this->handle, this->buffer_size))
			{
				if (!this->loops)
				{
					processed = i;
					break;
				}
				else if (this->loops > 0)
					--this->loops;
				Decoder::reset(this->handle);
				Decoder::read(this->buffer, this->handle, this->buffer_size);
			}
			unsigned int buffer = 0;
			alSourceUnqueueBuffers(this->sid, 1, &buffer);
			alBufferData(buffer, this->format, this->buffer, this->buffer_size, this->rate);
			alSourceQueueBuffers(this->sid, 1, &buffer);
		}
		if (processed > 0)
		{
			alGetSourcei(this->sid, AL_SOURCE_STATE, &processed);
			if(processed != AL_PLAYING)
				alSourcePlay(this->sid);
		}
	}

	bool Stream::load(const char *const file)
	{
		int type = 0;
		alGetSourcei(this->sid, AL_SOURCE_TYPE, &type);
		if (type != AL_UNDETERMINED)
			this->release();

		// Load file
		this->buffer_size = Decoder::open_stream(&this->handle, &this->buffer, this->format, this->rate, file);
		if (!this->buffer_size)
			return false;
		this->format = (this->format == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
		return this->preload();
	}

	void Stream::set_gain(const float gain)
	{
		alSourcef(this->sid, AL_GAIN, gain);
	}

	void Stream::set_loops(const int loops)
	{
		this->loops = loops;
	}

	void Stream::pause()
	{
		int state = 0;
		alGetSourcei(this->sid, AL_SOURCE_STATE, &state);
		if (state == AL_STOPPED)
			return;

		alSourcePause(this->sid);
		this->playing = false;
	}

	void Stream::play(const float x, const float y, const float z)
	{
		if (!this->buffer_size)
			return;

		int state = 0;
		alGetSourcei(this->sid, AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING)
			return;

		int queued;
		alGetSourcei(this->sid, AL_BUFFERS_QUEUED, &queued);
		if (!queued)
			this->preload();

		alSource3f(this->sid, AL_POSITION, x, y, z);
		alSourcePlay(this->sid);
		this->playing = true;
	}

	void Stream::stop()
	{
		alSourceStop(this->sid);

		int buffers;
		alGetSourcei(this->sid, AL_BUFFERS_PROCESSED, &buffers);
		if (buffers > 0)
			alSourceUnqueueBuffers(this->sid, buffers, this->bid);
		R_ASSERT(alGetError() == AL_NO_ERROR, "[Rainbow::ConFuoco] AL: Failed to unqueue buffers");

		this->playing = false;
	}

	bool Stream::preload()
	{
		for (unsigned int i = 0; i < CONFUOCO_STREAM_AL_BUFFERS; ++i)
		{
			Decoder::read(this->buffer, this->handle, this->buffer_size);
			alBufferData(this->bid[i], this->format, this->buffer, this->buffer_size, this->rate);
			if (alGetError() != AL_NO_ERROR)
			{
				fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to buffer data\n");
				return false;
			}
		}
		alSourceQueueBuffers(this->sid, CONFUOCO_STREAM_AL_BUFFERS, this->bid);
		return true;
	}

	void Stream::release()
	{
		this->stop();

		Decoder::close(this->handle);
		delete[] this->buffer;
		this->buffer = nullptr;
	}
}

#endif
