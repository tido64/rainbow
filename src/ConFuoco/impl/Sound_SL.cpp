// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include "ConFuoco/impl/Sound_SL.h"

namespace ConFuoco
{
	Sound::Sound(const unsigned int i) :
		loaded(false), paused(false), count(i), current(0),
		players(new AudioPlayer[this->count]) { }

	Sound::~Sound()
	{
		this->release();
		delete[] this->players;
	}

	bool Sound::load(const char *const file)
	{
		this->release();
		for (size_t i = 0; i < this->count; ++i)
		{
			if (!this->players[i].load(file))
			{
				this->release();
				return false;
			}
		}
		this->loaded = true;
		return true;
	}

	void Sound::set_gain(const float gain)
	{
		for (size_t i = 0; i < this->count; ++i)
			this->players[i].set_gain(gain);
	}

	void Sound::set_loops(const int) { }

	void Sound::pause()
	{
		if (this->paused)
			return;

		for (size_t i = 0; i < this->count; ++i)
			if (this->players[i].get_state() == SL_PLAYSTATE_PLAYING)
				this->players[i].pause();
		this->paused = true;
	}

	void Sound::play(const float, const float, const float)
	{
		if (this->paused)
		{
			for (size_t i = 0; i < this->count; ++i)
				if (this->players[i].get_state() == SL_PLAYSTATE_PAUSED)
					this->players[i].play();
			this->paused = false;
		}
		else
		{
			AudioPlayer &p = this->players[this->current];
			p.stop();
			p.play();
			++this->current %= this->count;
		}
	}

	void Sound::stop()
	{
		for (size_t i = 0; i < this->count; ++i)
			this->players[i].stop();
		this->paused = false;
	}

	void Sound::release()
	{
		if (this->loaded)
		{
			for (size_t i = 0; i < this->count; ++i)
				this->players[i].release();
			this->current = 0;
			this->loaded = false;
		}
	}
}

#endif
