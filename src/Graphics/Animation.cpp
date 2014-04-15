// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"

Animation::Animation(const Sprite::Ref &sprite,
                     const unsigned int *const frames,
                     const unsigned int fps,
                     const int delay)
    : TimedEvent(1000.0f / fps), frame(0), sprite(sprite), frames(frames),
      delay(delay), idled(0) { }

void Animation::set_frames(const unsigned int *const frames)
{
	this->frames.reset(frames);
	this->reset();
}

void Animation::tick()
{
	this->sprite->set_texture(this->frames[this->frame]);
	if (this->frames[this->frame + 1] == kAnimationEnd)
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
