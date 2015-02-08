// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_TIMINGFUNCTIONS_H_
#define SCRIPT_TIMINGFUNCTIONS_H_

namespace rainbow
{
	namespace timing
	{
		const int kInterval = 4;

		inline float cubic(const float t)
		{
			return t * t * t;
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

		inline float ease_in_cubic(const float a, const float b, const float t)
		{
			return linear(a, b, cubic(t));
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

		inline float ease_out_cubic(const float a, const float b, const float t)
		{
			return linear(a, b, flip(cubic, t));
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

		template<typename F>
		float ease_in_out(F&& f, const float a, const float b, const float t)
		{
			return t < 0.5f ? linear(a, b, f(t * 2.0f) * 0.5f)
			                : linear(a, b, 1.0f - f((1.0f - t) * 2.0f) * 0.5f);
		}

		inline
		float ease_in_out_cubic(const float a, const float b, const float t)
		{
			return ease_in_out(cubic, a, b, t);
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
	}
}

#endif
