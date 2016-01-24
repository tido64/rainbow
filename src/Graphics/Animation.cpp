// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"

Animation::Animation(const Sprite::Ref& sprite,
                     Frames frames,
                     unsigned int fps,
                     int delay)
    : stopped_(true), accumulated_(0), interval_(1000 / fps), frame_(0),
      frames_(std::move(frames)), sprite_(sprite), delay_(delay), idled_(0) {}

void Animation::set_frame_rate(unsigned int fps)
{
    const float progress = static_cast<float>(accumulated_) / interval_;
    interval_ = 1000 / fps;
    accumulated_ = interval_ * progress;
}

void Animation::set_frames(Frames frames)
{
    frames_ = std::move(frames);
    rewind();
}

void Animation::set_sprite(const Sprite::Ref& sprite)
{
    sprite_ = sprite;
    if (!is_stopped())
        set_current_frame();
}

void Animation::jump_to(unsigned int frame)
{
    // TODO: Check for invalid frame.
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
        callback_(this, Event::Start);
}

void Animation::stop()
{
    if (is_stopped())
        return;

    stopped_ = true;
    if (callback_)
        callback_(this, Event::End);
}

void Animation::update(unsigned long dt)
{
    if (is_stopped())
        return;

    accumulated_ += std::min<unsigned int>(dt, interval_ * 5);
    while (accumulated_ >= interval_)
    {
        // Consume first as |accumulated_| may be reset during a tick, leading
        // to an underflow.
        accumulated_ -= interval_;
        tick();
    }
}

void Animation::set_current_frame()
{
    if (!frames_ || !sprite_)
        return;

    sprite_->set_texture(frames_[frame_]);
}

void Animation::tick()
{
    if (frames_[frame_ + 1] == kAnimationEnd)
    {
        if (delay_ < 0)
        {
            stop();
            return;
        }
        if (idled_ == 0 && callback_)
        {
            callback_(this, Event::Complete);
            if (is_stopped())
                return;
        }
        if (idled_ < delay_)
        {
            ++idled_;
            return;
        }
        frame_ = 0;
        idled_ = 0;
    }
    else
    {
        ++frame_;
        if (callback_)
            callback_(this, Event::Frame);
    }
    sprite_->set_texture(frames_[frame_]);
}
