#ifndef TRANSITIONS_H_
#define TRANSITIONS_H_

#include <cassert>
#include <cmath>
#include "Common/Constants.h"

namespace Rainbow
{
	namespace Transitions
	{
		enum Effect
		{
			fx_linear,
			fx_easein_cubic,
			fx_easein_quadratic,
			fx_easein_square,
			fx_easeout_cubic,
			fx_easeout_quadratic,
			fx_easeout_square
		};

		/// Standard linear transition.
		class Linear
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * t;
			}
		};

		class EaseInCubic
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * t * t * t;
			}
		};

		class EaseInQuad
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * t * t * t * t;
			}
		};

		class EaseInSquare
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * t * t;
			}
		};

		class EaseOutCubic
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * pow(t, kThird);
			}
		};

		class EaseOutQuad
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * pow(t, kQuarter);
			}
		};

		class EaseOutSquare
		{
		public:
			float operator()(const float start, const float end, const float t) const
			{
				assert(t >= 0.0f && t < 1.0f);
				return start + (end - start) * sqrt(t);
			}
		};
	}
}

#endif
