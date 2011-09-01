/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Input/Input.h"

void Input::subscribe(Touchable *const t, unsigned int flags)
{
	//if (flags & RAINBOW_KEY_EVENTS)

	if (flags & RAINBOW_TOUCH_EVENTS)
		this->touch_subscribers.push_back(t);
}

void Input::touch_began(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_began(touches, count);
}

void Input::touch_canceled()
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_canceled();
}

void Input::touch_ended(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_ended(touches, count);
}

void Input::touch_moved(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_moved(touches, count);
}

void Input::touch_flip(Touch *const touches, const unsigned int count)
{
	const int height = static_cast<int>(Screen::Instance().height());
	for (unsigned int i = 0; i < count; ++i)
		touches[i].position.y = height - touches[i].position.y;
}
