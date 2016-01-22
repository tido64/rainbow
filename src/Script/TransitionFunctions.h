// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TRANSITIONFUNCTIONS_H_
#define SCRIPT_TRANSITIONFUNCTIONS_H_

#include <functional>

#include "Common/Color.h"
#include "Common/Vec2.h"

using TimingFunction = std::function<float(float, float, float)>;

namespace rainbow
{
    namespace timing
    {
        const int kInterval = 4;
    }

    inline int repeat_count_from_duration(int duration)
    {
        return std::ceil(static_cast<float>(duration) / timing::kInterval);
    }

    template <typename T, typename U>
    class Transition
    {
    protected:
        int elapsed_;
        int duration_;
        const U delta_;
        TimingFunction timing_;
        U previous_;
        T component_;

        Transition(T component,
                   const U& delta,
                   int duration,
                   TimingFunction timing)
            : elapsed_(0), duration_(duration), delta_(delta),
              timing_(std::move(timing)), component_(component) {}

        Transition(T component,
                   int outset,
                   const U& delta,
                   int duration,
                   TimingFunction timing)
            : elapsed_(0), duration_(duration), delta_(delta),
              timing_(std::move(timing)), previous_(outset),
              component_(component) {}

        float tick()
        {
            elapsed_ += timing::kInterval;
            return std::min(static_cast<float>(elapsed_) / duration_, 1.0f);
        }
    };

    template <typename T>
    class Fade : private Transition<T, int>
    {
    public:
        Fade(T component, int opacity, int duration, TimingFunction timing)
            : Transition<T, int>(
                  component, 0, opacity, duration, std::move(timing)) {}

        void operator()()
        {
            Colorb color = this->component_->color();
            color.a += this->timing_(0.0f, this->delta_, this->tick()) -
                       this->previous_;
            this->component_->set_color(color);
            this->previous_ = color.a;
        }
    };

    template <typename T>
    class Move : private Transition<T, Vec2f>
    {
    public:
        Move(T component,
             const Vec2f& delta,
             int duration,
             TimingFunction timing)
            : Transition<T, Vec2f>(
                  component, delta, duration, std::move(timing)) {}

        void operator()()
        {
            const float progress = this->tick();
            const Vec2f d(this->timing_(0.0f, this->delta_.x, progress),
                          this->timing_(0.0f, this->delta_.y, progress));
            this->component_->move(
                Vec2f(d.x - this->previous_.x, d.y - this->previous_.y));
            this->previous_ = d;
        }
    };

    template <typename T>
    class Rotate : private Transition<T, float>
    {
    public:
        Rotate(T component, float r, int duration, TimingFunction timing)
            : Transition<T, float>(component, 0, r, duration, std::move(timing))
        {
        }

        void operator()()
        {
            const float r = this->timing_(0.0f, this->delta_, this->tick());
            this->component_->rotate(r - this->previous_);
            this->previous_ = r;
        }
    };

    template <typename T>
    class Scale : private Transition<T, Vec2f>
    {
    public:
        Scale(T component, const Vec2f& f, int duration, TimingFunction timing)
            : Transition<T, Vec2f>(component, f, duration, std::move(timing)) {}

        void operator()()
        {
            const float progress = this->tick();
            const Vec2f d(this->timing_(0.0f, this->delta_.x, progress),
                          this->timing_(0.0f, this->delta_.y, progress));
            Vec2f scale = this->component_->scale();
            scale += Vec2f(d.x - this->previous_.x, d.y - this->previous_.y);
            this->component_->set_scale(scale);
            this->previous_ = d;
        }
    };
}

#endif
