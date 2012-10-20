// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#ifdef RAINBOW_ANDROID

#include "ConFuoco/impl/Stream_SL.h"

namespace ConFuoco
{
	/// Support looping a given number of times. The SL_IID_SEEK interface
	/// currently does not support this.
	void PlayCallback(SLPlayItf caller, void *pContext, SLuint32)
	{
		int &loops = *static_cast<int*>(pContext);
		if (loops == 0)
			return;
		if (loops > 0)
			--loops;

		(*caller)->SetPlayState(caller, SL_PLAYSTATE_STOPPED);
		(*caller)->SetPlayState(caller, SL_PLAYSTATE_PLAYING);
	}

	Stream::Stream() : loops(-1) { }

	Stream::~Stream() { }

	bool Stream::load(const char *const file)
	{
		this->release();
		if (!this->player.load(file)
		    || (*this->player.play_itf)->RegisterCallback(this->player.play_itf, PlayCallback, &this->loops) != SL_RESULT_SUCCESS
		    || (*this->player.play_itf)->SetCallbackEventsMask(this->player.play_itf, SL_PLAYEVENT_HEADATEND) != SL_RESULT_SUCCESS)
		{
			this->release();
			return false;
		}
		return true;
	}

	void Stream::set_gain(const float gain)
	{
		this->player.set_gain(gain);
	}

	void Stream::set_loops(const int loops)
	{
		this->loops = loops;
	}

	void Stream::pause()
	{
		this->player.pause();
	}

	void Stream::play(const float, const float, const float)
	{
		this->player.play();
	}

	void Stream::stop()
	{
		this->player.stop();
	}

	void Stream::release()
	{
		this->player.release();
	}
}

#endif
