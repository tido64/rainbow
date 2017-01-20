// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TIMINGFUNCTIONS_H_
#define SCRIPT_TIMINGFUNCTIONS_H_

#include <cmath>

#include "Common/Constants.h"

namespace rainbow { namespace timing
{
    inline auto back(float t)
    {
        return t * t * (2.70158f * t - 1.70158f);
    }

    inline auto bounce(float t)
    {
        if (t < (1.0f / 2.75f))
            return 7.5625f * t * t;

        if (t < (2.0f / 2.75f))
        {
            t -= 1.5f / 2.75f;
            return 7.5625f * t * t + .75f;
        }

        if (t < (2.5f / 2.75f))
        {
            t -= 2.25f / 2.75f;
            return 7.5625f * t * t + .9375f;
        }

        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + .984375f;
    }

    inline auto cubic(float t)
    {
        return t * t * t;
    }

    inline auto exponential(float t)
    {
        return std::pow(2, 10 * (t - 1));
    }

    template <typename F>
    auto flip(F&& f, float t) -> float
    {
        return 1.0f - f(1.0f - t);
    }

    inline auto quadratic(float t)
    {
        return t * t;
    }

    inline auto quartic(float t)
    {
        return t * t * t * t;
    }

    inline auto quintic(float t)
    {
        return t * t * t * t * t;
    }

    inline auto linear(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    inline auto ease_in_back(float a, float b, float t)
    {
        return linear(a, b, back(t));
    }

    inline auto ease_in_bounce(float a, float b, float t)
    {
        return linear(a, b, flip(bounce, t));
    }

    inline auto ease_in_cubic(float a, float b, float t)
    {
        return linear(a, b, cubic(t));
    }

    inline auto ease_in_exponential(float a, float b, float t)
    {
        return linear(a, b, exponential(t));
    }

    inline auto ease_in_quadratic(float a, float b, float t)
    {
        return linear(a, b, quadratic(t));
    }

    inline auto ease_in_quartic(float a, float b, float t)
    {
        return linear(a, b, quartic(t));
    }

    inline auto ease_in_quintic(float a, float b, float t)
    {
        return linear(a, b, quintic(t));
    }

    inline auto ease_in_sine(float a, float b, float t)
    {
        const float c = b - a;
        return a + c - c * std::cos(t * kPi_2<float>);
    }

    inline auto ease_out_back(float a, float b, float t)
    {
        return linear(a, b, flip(back, t));
    }

    inline auto ease_out_bounce(float a, float b, float t)
    {
        return linear(a, b, bounce(t));
    }

    inline auto ease_out_cubic(float a, float b, float t)
    {
        return linear(a, b, flip(cubic, t));
    }

    inline auto ease_out_exponential(float a, float b, float t)
    {
        return linear(a, b, flip(exponential, t));
    }

    inline auto ease_out_quadratic(float a, float b, float t)
    {
        return linear(a, b, flip(quadratic, t));
    }

    inline auto ease_out_quartic(float a, float b, float t)
    {
        return linear(a, b, flip(quartic, t));
    }

    inline auto ease_out_quintic(float a, float b, float t)
    {
        return linear(a, b, flip(quintic, t));
    }

    inline auto ease_out_sine(float a, float b, float t)
    {
        return a + (b - a) * std::sin(t * kPi_2<float>);
    }

    template <typename F>
    float ease_in_out(F&& f, float a, float b, float t)
    {
        return (t < 0.5f ? linear(a, b, f(t * 2.0f) * 0.5f)
                         : linear(a, b, 1.0f - f((1.0f - t) * 2.0f) * 0.5f));
    }

    inline auto ease_in_out_back(float a, float b, float t)
    {
        return ease_in_out(back, a, b, t);
    }

    inline auto ease_in_out_bounce(float a, float b, float t)
    {
        return (t < 0.5f ? ease_in_bounce(a, b, t * 2.0f) * 0.5f
                         : ease_out_bounce(a, b, (t - 0.5) * 2.0f) * 0.5f +
                               (b - a) * 0.5f);
    }

    inline auto ease_in_out_cubic(float a, float b, float t)
    {
        return ease_in_out(cubic, a, b, t);
    }

    inline auto ease_in_out_exponential(float a, float b, float t)
    {
        return ease_in_out(exponential, a, b, t);
    }

    inline auto ease_in_out_quadratic(float a, float b, float t)
    {
        return ease_in_out(quadratic, a, b, t);
    }

    inline auto ease_in_out_quartic(float a, float b, float t)
    {
        return ease_in_out(quartic, a, b, t);
    }

    inline auto ease_in_out_quintic(float a, float b, float t)
    {
        return ease_in_out(quintic, a, b, t);
    }

    inline auto ease_in_out_sine(float a, float b, float t)
    {
        return a - (b - a) / 2.0f * (std::cos(kPi<float> * t) - 1.0f);
    }
}}  // namespace rainbow::timing

#endif
