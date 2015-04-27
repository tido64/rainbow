// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
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
    : stopped_(true), accumulated_(0), interval_(1000 / fps), frame_(0),
      frames_(frames), sprite_(sprite), delay_(delay), idled_(0) {}

void Animation::set_fps(const unsigned int fps)
{
	accumulated_ = 0;
	interval_ = 1000 / fps;
}

void Animation::set_frames(const Frame *const frames)
{
	frames_.reset(frames);
	rewind();
}

void Animation::set_sprite(const Sprite::Ref &sprite)
{
	sprite_ = sprite;
	if (!is_stopped())
	{
		if (frame_ > 0)
			sprite_->set_texture(frames_[frame_ - 1]);
		else
			set_current_frame();
	}
}

void Animation::jump_to(const unsigned int frame)
{
	accumulated_ = 0;
	frame_ = frame;
	idled_ = 0;
	set_current_frame();
}

void Animation::start()
{
	if (!is_stopped())
		return;

	stopped_ = false;
	rewind();
	if (callback_)
		callback_(Event::Start);
}

void Animation::stop()
{
	if (is_stopped())
		return;

	stopped_ = true;
	if (callback_)
		callback_(Event::End);
}

void Animation::update(const unsigned long dt)
{
	if (is_stopped())
		return;

	accumulated_ += std::min<unsigned int>(dt, interval_ * 5);
	while (accumulated_ >= interval_)
	{
		tick();
		accumulated_ -= interval_;
	}
}

void Animation::set_current_frame()
{
	if (!frames_ || !sprite_)
		return;

	sprite_->set_texture(frames_[frame_]);
	if (frames_[frame_ + 1] != kAnimationEnd)
		++frame_;
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
		{
			frame_ = 0;
			idled_ = 0;
		}
	}
	else
		++frame_;
}
