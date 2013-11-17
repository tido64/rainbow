// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
