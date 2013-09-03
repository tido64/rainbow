// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Graphics/Animation.h"
#include "Graphics/Sprite.h"

void Animation::set_frames(const uint_t *const frames)
{
	this->frames.reset(frames);
	this->reset();
}

void Animation::tick()
{
	this->sprite->set_texture(*this->frame);
	if (*(this->frame + 1) == kAnimationEnd)
	{
		if (this->delay < 0)
			this->stop();
		else if (this->idled < this->delay)
			++this->idled;
		else
			this->reset();
	}
	else
		++this->frame;
}
