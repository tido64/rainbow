// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include <cstring>

#include "Common/Debug.h"
#include "ConFuoco/impl/Stream_SL.h"

namespace ConFuoco
{
	const SLInterfaceID iids[] = { SL_IID_VOLUME,   SL_IID_BUFFERQUEUE };
	const SLboolean req[]      = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	void BufferQueueCallback(SLBufferQueueItf buffer_queue_itf, void *context)
	{
		char *buffer;
		size_t size = static_cast<Stream*>(context)->get(buffer);
		if (!buffer || !size)
			return;
		(*buffer_queue_itf)->Enqueue(buffer_queue_itf, buffer, size);
	}

	Stream::Stream() :
		loops(-1), buffer_ptr(0), file(nullptr),
		player(nullptr), play_itf(nullptr), volume_itf(nullptr),
		buffer_queue_itf(nullptr)
	{
		memset(this->buffers, 0, sizeof(this->buffers));
	}

	Stream::~Stream()
	{
		this->release();
	}

	size_t Stream::get(char *&buffer)
	{
		buffer = this->buffers[this->buffer_ptr];
		++this->buffer_ptr %= kAudioBuffers;

		size_t read = fread(buffer, 1, kAudioBufferSize, this->file);
		if (read != kAudioBufferSize)
		{
			if (!feof(this->file))
			{
				this->release();
				buffer = nullptr;
				return 0u;
			}
			if (this->loops)
			{
				if (this->loops > 0)
					--this->loops;
				rewind(this->file);
				read += fread(buffer + read, 1, kAudioBufferSize - read, this->file);
			}
		}
		return read;
	}

	bool Stream::load(const char *const file)
	{
		this->release();

		this->file = fopen(file, "rb");
		if (!this->file)
			return false;

		short header[2];
		fread(header, 1, 4, this->file);

		SLDataLocator_BufferQueue buffer_queue;
		buffer_queue.locatorType = SL_DATALOCATOR_BUFFERQUEUE;
		buffer_queue.numBuffers = kAudioBuffers;

		SLDataFormat_MIME format;
		format.formatType = SL_DATAFORMAT_MIME;
		if ((header[0] & 0xfffe) == 0xfffa || memcmp(header, "ID3", 3) == 0)
		{
			format.mimeType = (SLchar*)"audio/mpeg";
			format.containerType = SL_CONTAINERTYPE_MP3;
		}
		else if (memcmp(header, "RIFF", 4) == 0)
		{
			format.mimeType = (SLchar*)"audio/x-wav";
			format.containerType = SL_CONTAINERTYPE_WAV;
		}
		else if ((header[0] & 0xffe0) == 0x56e0 || header[0] == 0x4de1)
		{
			format.mimeType = (SLchar*)"audio/MP4A-LATM";
			format.containerType = SL_CONTAINERTYPE_AAC;
		}
		else if (memcmp(header, "OggS", 4) == 0)
		{
			format.mimeType = (SLchar*)"application/ogg";
			format.containerType = SL_CONTAINERTYPE_OGG;
		}

		SLDataSource source;
		source.pFormat = &format;
		source.pLocator = &buffer_queue;

		SLDataLocator_OutputMix output_mix;
		output_mix.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		//output_mix.outputMix = &Mixer::Engine();

		SLDataSink sink;
		sink.pLocator = &output_mix;
		sink.pFormat = nullptr;

		//(*Mixer::Engine())->CreateAudioPlayer(Mixer::Engine(), &this->player, &source, &sink, 2, iids, req);
		if ((*this->player)->Realize(this->player, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS
		    || (*this->player)->GetInterface(this->player, SL_IID_PLAY, &this->play_itf) != SL_RESULT_SUCCESS
		    || (*this->player)->GetInterface(this->player, SL_IID_VOLUME, &this->volume_itf) != SL_RESULT_SUCCESS
		    || (*this->player)->GetInterface(this->player, SL_IID_BUFFERQUEUE, &this->buffer_queue_itf) != SL_RESULT_SUCCESS
		    || (*this->buffer_queue_itf)->RegisterCallback(this->buffer_queue_itf, BufferQueueCallback, this) != SL_RESULT_SUCCESS)
		{
			this->release();
			return false;
		}

		this->preload();
		return true;
	}

	void Stream::set_gain(const float gain)
	{
		(*this->volume_itf)->SetVolumeLevel(this->volume_itf, (1.0f - gain) * -100);
	}

	void Stream::set_loops(const int loops)
	{
		this->loops = loops;
	}

	void Stream::pause()
	{
		if (!this->play_itf)
			return;

		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_PAUSED);
	}

	void Stream::play(const float, const float, const float)
	{
		if (!this->play_itf)
			return;

		SLuint32 state;
		(*this->play_itf)->GetPlayState(this->play_itf, &state);
		if (state == SL_PLAYSTATE_STOPPED)
		{
			SLBufferQueueState state;
			(*this->buffer_queue_itf)->GetState(this->buffer_queue_itf, &state);
			if (state.count == 0)
				this->preload();
		}
		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_PLAYING);
	}

	void Stream::stop()
	{
		if (!this->play_itf)
			return;

		(*this->play_itf)->SetPlayState(this->play_itf, SL_PLAYSTATE_STOPPED);
		(*this->buffer_queue_itf)->Clear(this->buffer_queue_itf);
		rewind(this->file);
	}

	void Stream::preload()
	{
		R_ASSERT(this->buffer_queue_itf, "Missing buffer queue interface");

		this->buffer_ptr = 0;
		for (size_t i = 0; i < kAudioBuffers; ++i)
			BufferQueueCallback(this->buffer_queue_itf, this);
	}

	void Stream::release()
	{
		if (this->player)
		{
			this->stop();
			(*this->player)->Destroy(this->player);
			this->player = nullptr;
			this->play_itf = nullptr;
			this->volume_itf = nullptr;
			this->buffer_queue_itf = nullptr;
			memset(this->buffers, 0, sizeof(this->buffers));
		}
		if (this->file)
		{
			fclose(this->file);
			this->file = nullptr;
		}
	}
}

#endif
