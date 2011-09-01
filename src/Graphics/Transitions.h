#ifndef TRANSITIONS_H_
#define TRANSITIONS_H_

#include <cmath>

namespace Transitions
{
	class EaseInCubic
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t * t;
		}
	};

	class EaseInQuad
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t * t * t;
		}
	};

	class EaseInSquare
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * t * t;
		}
	};

	class EaseOutCubic
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * pow(t, kThird);
		}
	};

	class EaseOutQuad
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * pow(t, kQuarter);
		}
	};

	class EaseOutSquare
	{
	public:
		float operator()(const float start, const float end, const float t)
		{
			assert(t >= 0.0f && t < 1.0f);
			return start + (end - start) * sqrt(t);
		}
	};
}

#endif
