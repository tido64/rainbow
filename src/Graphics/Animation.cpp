/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Algorithm.h"
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

void Animation::tick()
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
