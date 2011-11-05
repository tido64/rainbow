#ifndef TRANSITION_H_
#define TRANSITION_H_

#include "Graphics/Animation.h"

/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class Transition : public Animation
{
public:
	Transition(float &property, const float final, const unsigned int duration = 1000);
	virtual bool update();

private:
	T transition;
	const float final;
	const float initial;
	float &property;
};

template<class T>
inline Transition<T>::Transition(float &prop, const float fin, const unsigned int dur) :
	Animation(dur), final(fin), initial(prop), property(prop) { }

template<>
class Transition<void>
{
public:
	static Animation* create(float &property, const float final, const unsigned int duration, const int transition);
};

#endif
