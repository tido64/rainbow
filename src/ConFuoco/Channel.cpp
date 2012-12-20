#include "ConFuoco/Mixer.h"

namespace ConFuoco
{
	bool Channel::is_paused()
	{
		return this->mixer->is_paused(this);
	}

	bool Channel::is_playing()
	{
		return this->mixer->is_playing(this);
	}

	void Channel::set_gain(const float gain)
	{
		if (this->ch == static_cast<unsigned int>(-1))
			R_ERROR("[Rainbow::ConFuoco] No sound set to this channel");

		this->mixer->set_gain(this, gain);
	}

	void Channel::pause()
	{
		R_ASSERT(this->ch != static_cast<unsigned int>(-1),
		         "No sound set to this channel");

		if (this->is_paused())
			this->mixer->play(this);
		else
			this->mixer->pause(this);
	}

	void Channel::stop()
	{
		R_ASSERT(this->ch != static_cast<unsigned int>(-1),
		         "No sound set to this channel");

		this->mixer->stop(this);
	}

	Channel::Channel() : ch(-1), mixer(nullptr), sound(nullptr) { }
}

