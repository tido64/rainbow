// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TRANSITION_H_
#define SCRIPT_TRANSITION_H_

#include "Script/Timer.h"
#include "Script/TimingFunctions.h"
#include "Script/TransitionFunctions.h"

namespace rainbow
{
    class SceneNode;

    inline auto move(rainbow::SceneNode* node,
                     const Vec2f& delta,
                     int duration,
                     TimingFunction timing)
    {
        return TimerManager::Get()->set_timer(
            Move<rainbow::SceneNode*>(node, delta, duration, std::move(timing)),
            timing::kInterval,
            repeat_count_from_duration(duration));
    }

    template <typename T>
    auto fade(T component, int opacity, int duration, TimingFunction timing)
    {
        opacity -= component->color().a;
        return TimerManager::Get()->set_timer(
            Fade<T>(component, opacity, duration, std::move(timing)),
            timing::kInterval,
            repeat_count_from_duration(duration));
    }

    template <typename T>
    auto fade(T component, float opacity, int duration, TimingFunction timing)
    {
        return fade(component,
                    static_cast<int>(opacity * 255.0f + 0.5f),
                    duration,
                    std::move(timing));
    }

    template <typename T>
    auto move(T component,
              Vec2f destination,
              int duration,
              TimingFunction timing)
    {
        destination -= component->position();
        return TimerManager::Get()->set_timer(
            Move<T>(component, destination, duration, std::move(timing)),
            timing::kInterval,
            repeat_count_from_duration(duration));
    }

    template <typename T>
    auto rotate(T component, float angle, int duration, TimingFunction timing)
    {
        angle -= component->angle();
        return TimerManager::Get()->set_timer(
            Rotate<T>(component, angle, duration, std::move(timing)),
            timing::kInterval,
            repeat_count_from_duration(duration));
    }

    template <typename T>
    auto scale(T component, Vec2f factor, int duration, TimingFunction timing)
    {
        factor -= component->scale();
        return TimerManager::Get()->set_timer(
            Scale<T>(component, factor, duration, std::move(timing)),
            timing::kInterval,
            repeat_count_from_duration(duration));
    }

    template <typename T>
    auto scale(T component, float factor, int duration, TimingFunction timing)
    {
        return scale(
            component, Vec2f(factor, factor), duration, std::move(timing));
    }
}

#endif
