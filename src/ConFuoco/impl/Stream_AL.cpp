// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/OpenAL.h"
#ifdef RAINBOW_SDL

#include <cstdio>

#include "Common/RainbowAssert.h"
#include "ConFuoco/Decoder.h"
#include "ConFuoco/impl/Stream_AL.h"

namespace ConFuoco
{
	Stream::Stream() :
		format(0), rate(0), loops(-1), buffer_size(0), buffer(nullptr), handle(nullptr)
	{
		R_ASSERT(this->sid, "Failed to generate source.");

		alGenBuffers(CONFUOCO_STREAM_AL_BUFFERS, this->bid);
		if (alGetError() != AL_NO_ERROR)
		{
			alDeleteSources(1, &this->sid);
			this->sid = 0;
			fprintf(stderr, "Rainbow::ConFuoco::AL: Failed to generate buffers\n");
			return;
		}
	}

	Stream::~Stream()
	{
		if (!this->sid)
			return;

		if (this->handle)
			this->release();
		alDeleteBuffers(CONFUOCO_STREAM_AL_BUFFERS, this->bid);
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

	void Stream::set_loops(const int loops)
	{
		this->loops = loops;
	}

	void Stream::update()
	{
		int processed = 0;
		alGetSourcei(this->sid, AL_BUFFERS_PROCESSED, &processed);
		for (int i = 0; i < processed; ++i)
		{
			if (!Decoder::read(this->buffer, this->handle, this->buffer_size))
			{
				if (this->loops > 0)
				{
					if (!this->loops)
						break;

					--this->loops;
				}
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
		alSourceUnqueueBuffers(this->sid, CONFUOCO_STREAM_AL_BUFFERS, this->bid);
		Decoder::close(this->handle);
		delete[] this->buffer;
		this->buffer = nullptr;
	}
}

#endif
