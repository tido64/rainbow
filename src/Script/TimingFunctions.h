// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TIMINGFUNCTIONS_H_
#define SCRIPT_TIMINGFUNCTIONS_H_

#include <cmath>

#include "Common/Constants.h"

namespace rainbow
{
	namespace timing
	{
		inline float back(const float t)
		{
			return t * t * (2.70158f * t - 1.70158f);
		}

		inline float bounce(float t)
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

		inline float cubic(const float t)
		{
			return t * t * t;
		}

		inline float exponential(const float t)
		{
			return std::pow(2, 10 * (t - 1));
		}

		template<typename F>
		float flip(F&& f, const float t)
		{
			return 1.0f - f(1.0f - t);
		}

		inline float quadratic(const float t)
		{
			return t * t;
		}

		inline float quartic(const float t)
		{
			return t * t * t * t;
		}

		inline float quintic(const float t)
		{
			return t * t * t * t * t;
		}

		inline float linear(const float a, const float b, const float t)
		{
			return a + (b - a) * t;
		}

		inline float ease_in_back(const float a, const float b, const float t)
		{
			return linear(a, b, back(t));
		}

		inline float ease_in_bounce(const float a, const float b, float t)
		{
			return linear(a, b, flip(bounce, t));
		}

		inline float ease_in_cubic(const float a, const float b, const float t)
		{
			return linear(a, b, cubic(t));
		}

		inline
		float ease_in_exponential(const float a, const float b, const float t)
		{
			return linear(a, b, exponential(t));
		}

		inline
		float ease_in_quadratic(const float a, const float b, const float t)
		{
			return linear(a, b, quadratic(t));
		}

		inline
		float ease_in_quartic(const float a, const float b, const float t)
		{
			return linear(a, b, quartic(t));
		}

		inline
		float ease_in_quintic(const float a, const float b, const float t)
		{
			return linear(a, b, quintic(t));
		}

		inline float ease_in_sine(const float a, const float b, const float t)
		{
			const float c = b - a;
			return a + c - c * cos(t * static_cast<float>(kPi_2));
		}

		inline float ease_out_back(const float a, const float b, const float t)
		{
			return linear(a, b, flip(back, t));
		}

		inline float ease_out_bounce(const float a, const float b, float t)
		{
			return linear(a, b, bounce(t));
		}

		inline float ease_out_cubic(const float a, const float b, const float t)
		{
			return linear(a, b, flip(cubic, t));
		}

		inline
		float ease_out_exponential(const float a, const float b, const float t)
		{
			return linear(a, b, flip(exponential, t));
		}

		inline
		float ease_out_quadratic(const float a, const float b, const float t)
		{
			return linear(a, b, flip(quadratic, t));
		}

		inline
		float ease_out_quartic(const float a, const float b, const float t)
		{
			return linear(a, b, flip(quartic, t));
		}

		inline
		float ease_out_quintic(const float a, const float b, const float t)
		{
			return linear(a, b, flip(quintic, t));
		}

		inline float ease_out_sine(const float a, const float b, const float t)
		{
			return a + (b - a) * sin(t * static_cast<float>(kPi_2));
		}

		template<typename F>
		float ease_in_out(F&& f, const float a, const float b, const float t)
		{
			return (t < 0.5f
			            ? linear(a, b, f(t * 2.0f) * 0.5f)
			            : linear(a, b, 1.0f - f((1.0f - t) * 2.0f) * 0.5f));
		}

		inline
		float ease_in_out_back(const float a, const float b, const float t)
		{
			return ease_in_out(back, a, b, t);
		}

		inline
		float ease_in_out_bounce(const float a, const float b, const float t)
		{
			return (t < 0.5f ? ease_in_bounce(a, b, t * 2.0f) * 0.5f
			                 : ease_out_bounce(a, b, (t - 0.5) * 2.0f) * 0.5f +
			                       (b - a) * 0.5f);
		}

		inline
		float ease_in_out_cubic(const float a, const float b, const float t)
		{
			return ease_in_out(cubic, a, b, t);
		}

		inline float
		ease_in_out_exponential(const float a, const float b, const float t)
		{
			return ease_in_out(exponential, a, b, t);
		}

		inline
		float ease_in_out_quadratic(const float a, const float b, const float t)
		{
			return ease_in_out(quadratic, a, b, t);
		}

		inline
		float ease_in_out_quartic(const float a, const float b, const float t)
		{
			return ease_in_out(quartic, a, b, t);
		}

		inline
		float ease_in_out_quintic(const float a, const float b, const float t)
		{
			return ease_in_out(quintic, a, b, t);
		}

		inline
		float ease_in_out_sine(const float a, const float b, const float t)
		{
			return a - (b - a) / 2.0f * (cos(kPi * t) - 1.0f);
		}
	}
}

#endif
