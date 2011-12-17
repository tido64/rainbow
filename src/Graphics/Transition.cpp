/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Algorithm.h"
#include "Common/Chrono.h"
#include "Graphics/Transition.h"
#include "Graphics/Transitions.h"

using Rainbow::equalf;
using namespace Rainbow::Transitions;

Transition* Transition::create(float &property, const float final, const unsigned int duration, const int transition)
{
	Transition *t = nullptr;
	switch (transition)
	{
		case fx_easein_cubic:
			t = new Rainbow::_Transition<EaseInCubic>(property, final, duration);
			break;
		case fx_easein_quadratic:
			t = new Rainbow::_Transition<EaseInQuad>(property, final, duration);
			break;
		case fx_easein_square:
			t = new Rainbow::_Transition<EaseInSquare>(property, final, duration);
			break;
		case fx_easeout_cubic:
			t = new Rainbow::_Transition<EaseOutCubic>(property, final, duration);
			break;
		case fx_easeout_quadratic:
			t = new Rainbow::_Transition<EaseOutQuad>(property, final, duration);
			break;
		case fx_easeout_square:
			t = new Rainbow::_Transition<EaseOutSquare>(property, final, duration);
			break;
		default:
			t = new Rainbow::_Transition<Linear>(property, final, duration);
			break;
	}
	return t;
}

namespace Rainbow
{
	template<class T>
	bool _Transition<T>::update()
	{
		if (this->stopped)
			return true;

		this->elapsed += Chrono::Instance().diff();
		const float progress = static_cast<float>(this->elapsed) / static_cast<float>(this->duration);
		if (progress >= 1.0f)
		{
			this->property = this->final;
			this->stopped = true;
			return true;
		}
		this->property = this->transition(this->initial, this->final, progress);
		return false;
	}
}
