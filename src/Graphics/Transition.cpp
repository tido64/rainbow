/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Algorithm.h"
#include "Graphics/Transition.h"
#include "Graphics/Transitions.h"

using Rainbow::equalf;
using namespace Rainbow::Transitions;

Animation* Transition<void>::create(float &property, const float final, const unsigned int duration, const int transition)
{
	Animation *a = nullptr;
	switch (transition)
	{
		case fx_easein_cubic:
			a = new Transition<EaseInCubic>(property, final, duration);
			break;
		case fx_easein_quadratic:
			a = new Transition<EaseInQuad>(property, final, duration);
			break;
		case fx_easein_square:
			a = new Transition<EaseInSquare>(property, final, duration);
			break;
		case fx_easeout_cubic:
			a = new Transition<EaseOutCubic>(property, final, duration);
			break;
		case fx_easeout_quadratic:
			a = new Transition<EaseOutQuad>(property, final, duration);
			break;
		case fx_easeout_square:
			a = new Transition<EaseOutSquare>(property, final, duration);
			break;
		default:
			a = new Transition<Linear>(property, final, duration);
			break;
	}
	return a;
}

template<class T>
bool Transition<T>::update()
{
	const float progress = this->get_progress();
	if (equalf(progress, 1.0f))
	{
		this->property = this->final;
		return true;
	}
	this->property = this->transition(this->initial, this->final, progress);
	return false;
}
