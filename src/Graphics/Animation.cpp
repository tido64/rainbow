// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Common/Algorithm.h"
#include "Graphics/Animation.h"
#include "Graphics/Sprite.h"

using Rainbow::equalf;

Animation::~Animation()
{
	delete[] this->frames;
}

void Animation::set_frames(const unsigned int *const frames)
{
	delete[] this->frames;

	this->frames = frames;
	this->reset();
}

void Animation::tick_impl()
{
	this->sprite->set_texture(*this->frame);
	if (!*(this->frame + 1))
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
