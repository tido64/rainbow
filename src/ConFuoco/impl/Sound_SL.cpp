// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include <cstdio>

#include "ConFuoco/impl/Sound_SL.h"

namespace ConFuoco
{
	Sound::Sound(const unsigned int i) :
		paused(false), count(i), current(0), size(0), buffer(nullptr)
	{
		this->players = new Player[this->count];
	}

	Sound::~Sound()
	{
		this->release();
		delete[] this->players;
	}

	bool Sound::load(const char *const file)
	{
		return false;

		this->release();

		FILE *fd = fopen(file, "rb");
		if (!fd)
			return false;

		fseek(fd, 0, SEEK_END);
		this->size = ftell(fd);
		rewind(fd);

		this->buffer = new char[this->size];
		fread(this->buffer, 1, this->size, fd);
		fclose(fd);

		for (size_t i = 0; i < this->count; ++i)
		{
			/// stuff
		}

		return true;
	}

	void Sound::set_gain(const float gain)
	{
		if (!this->players)
			return;

		for (size_t i = 0; i < this->count; ++i)
		{
			SLVolumeItf &volume_itf = this->players[i].volume_itf;
			(*volume_itf)->SetVolumeLevel(volume_itf, (1.0f - gain) * -100);
		}
	}

	void Sound::set_loops(const int) { }

	void Sound::pause()
	{
		if (!this->players || this->paused)
			return;

		for (size_t i = 0; i < this->count; ++i)
		{
			Player &p = this->players[i];
			(*p.play_itf)->SetPlayState(p.play_itf, SL_PLAYSTATE_PAUSED);
		}
		this->paused = true;
	}

	void Sound::play(const float, const float, const float)
	{
		if (this->paused)
		{
			for (size_t i = 0; i < this->count; ++i)
			{
				SLuint32 state;
				SLPlayItf &play_itf = this->players[i].play_itf;
				(*play_itf)->GetPlayState(play_itf, &state);
				if (state == SL_PLAYSTATE_PAUSED)
					(*play_itf)->SetPlayState(play_itf, SL_PLAYSTATE_PLAYING);
			}
			this->paused = false;
		}
		else
		{
			SLuint32 state;
			Player &p = this->players[this->current];
			(*p.play_itf)->GetPlayState(p.play_itf, &state);
			if (state == SL_PLAYSTATE_PLAYING)
			{
				(*p.play_itf)->SetPlayState(p.play_itf, SL_PLAYSTATE_STOPPED);
				(*p.buffer_queue_itf)->Clear(p.buffer_queue_itf);
			}
			(*p.buffer_queue_itf)->Enqueue(p.buffer_queue_itf, this->buffer, this->size);
			(*p.play_itf)->SetPlayState(p.play_itf, SL_PLAYSTATE_PLAYING);
			++this->current %= this->count;
		}
	}

	void Sound::stop()
	{
		if (!this->players)
			return;

		for (size_t i = 0; i < this->count; ++i)
		{
			Player &p = this->players[i];
			(*p.play_itf)->SetPlayState(p.play_itf, SL_PLAYSTATE_STOPPED);
			(*p.buffer_queue_itf)->Clear(p.buffer_queue_itf);
		}
		this->paused = false;
	}

	void Sound::release()
	{
		this->stop();
		if (this->players)
		{
			for (size_t i = 0; i < this->count; ++i)
			{
				SLObjectItf &player = this->players[i].player;
				(*player)->Destroy(player);
			}
			memset(this->players, 0, this->count * sizeof(Player));
		}
		if (this->buffer)
		{
			delete[] this->buffer;
			this->buffer = nullptr;
		}
	}
}

#endif
