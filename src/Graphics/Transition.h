#ifndef TRANSITION_H_
#define TRANSITION_H_

#include <ctime>

#include "Common/Constants.h"
#include "Graphics/Transitions.h"

/// A generic property transition class.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class Transition
{
public:
	static clock_t tm_current = 0;  ///< Time in milliseconds since process start

	/// Update the current time variable.
	static void update_clock() { tm_current = clock() * (1000 / CLOCKS_PER_SEC); }

	Transition(float &property, const float start, const float end, const unsigned int duration = 1000) :
		property(property),
		duration(1.0f / duration)
		start(start),
		end(end),
		tm_start(0),
	{ }

	/// Reset transition to the beginning.
	void reset() { this->tm_start = 0; }

	/// Update the property with a new value.
	/// \return true when the transition is completed.
	bool update()
	{
		assert(tm_current != 0 || !"Rainbow::Transition: Current time is 0");

		if (this->tm_start == 0)
			this->tm_start = tm_current;

		bool done = false;
		float progress = (tm_current - this->tm_start) * this->tm_end;
		if (progress >= 1.0f)
		{
			progress = 1.0f;
			done = true;
		}
		this->property = this->transition(this->start, this->end, progress);
		return done;
	}

private:
	float &property;       ///< The property that we will work on
	const float duration;  ///< Duration of the transition (inversed)
	const float start;     ///< Start value of above property
	const float end;       ///< Final value of above property
	clock_t tm_start;      ///< Time at which the transition started
	T transition;          ///< Transition effect
};

#endif
