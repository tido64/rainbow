// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Common/Debug.h"
#include "ConFuoco/Sound.h"
#include "ConFuoco/Stream.h"
#include "ConFuoco/WaveBank.h"

namespace ConFuoco
{
	void WaveBank::clear()
	{
		for (unsigned int i = 0; i < this->sound_count; ++i)
			delete static_cast<Sound*>(this->bank[i]);
		this->sound_count = 0;

		Wave **bank = this->bank + size;
		for (unsigned int i = 0; i < this->stream_count; ++i)
			delete static_cast<Stream*>(bank[i]);
		this->stream_count = 0;
	}

	Sound* WaveBank::create_sound(const unsigned int instances)
	{
		if (this->sound_count + this->stream_count >= size)
		{
			R_ERROR("[Rainbow::ConFuoco] WaveBank: Max number of sources reached\n");
			return nullptr;
		}

		Sound *s = new Sound(instances);
		this->bank[this->sound_count] = s;
		++this->sound_count;
		return s;
	}

	Stream* WaveBank::create_stream()
	{
		if (this->sound_count + this->stream_count >= size)
		{
			R_ERROR("[Rainbow::ConFuoco] WaveBank: Max number of sources reached\n");
			return nullptr;
		}

		Stream *s = new Stream();
		this->bank[this->stream_count + size] = s;
		++this->stream_count;
		return s;
	}

	void WaveBank::remove(const Sound *s)
	{
		if (!this->sound_count)
			return;

		for (unsigned int i = this->sound_count - 1; i != 0; --i)
		{
			if (this->bank[i] == s)
			{
				this->bank[i] = this->bank[--this->sound_count];
				delete s;
				break;
			}
		}
	}

	void WaveBank::remove(const Stream *s)
	{
		if (!this->stream_count)
			return;

		Wave **bank = this->bank + size;
		for (unsigned int i = this->stream_count - 1; i != 0; --i)
		{
			if (bank[i] == s)
			{
				bank[i] = bank[--this->stream_count];
				delete s;
				break;
			}
		}
	}

	void WaveBank::update()
	{
		Wave **bank = this->bank + size;
		for (unsigned int i = 0; i < this->stream_count; ++i)
			static_cast<Stream*>(bank[i])->update();
	}
}
