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

	this->frame = frames;
	this->frames = frames;
}

void Animation::tick()
{
	this->sprite->set_texture(*this->frame);
	if (!*(++this->frame))
	{
		if (!this->loop)
			this->stop();
		else
			this->reset();
	}
}
