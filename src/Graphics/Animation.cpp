// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"

#include <limits>

// TODO: Move back to .h when MSVC finally implements constexpr.
const Animation::Frame Animation::kAnimationEnd =
    std::numeric_limits<Animation::Frame>::max();

Animation::Animation(const Sprite::Ref &sprite,
                     const Frame *const frames,
                     const unsigned int fps,
                     const int delay)
    : TimedEvent(1000.0f / fps), frame_(0), sprite_(sprite), frames_(frames),
      delay_(delay), idled_(0) {}

void Animation::set_frames(const Frame *const frames)
{
	frames_.reset(frames);
	reset();
}

void Animation::tick()
{
	sprite_->set_texture(frames_[frame_]);
	if (frames_[frame_ + 1] == kAnimationEnd)
	{
		if (delay_ < 0)
		{
			stop();
			return;
		}
		if (idled_ == 0 && callback_)
			callback_(Event::Complete);
		if (idled_ < delay_)
			++idled_;
		else
			reset();
	}
	else
		++frame_;
}

void Animation::on_start()
{
	reset();
	if (callback_)
		callback_(Event::Start);
}

void Animation::on_stop()
{
	if (callback_)
		callback_(Event::End);
}
