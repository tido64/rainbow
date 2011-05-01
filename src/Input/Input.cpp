//
//  Input.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 6/27/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "Input.h"

void Input::subscribe(Touchable *const t, unsigned int flags)
{
	//if (flags & RAINBOW_KEY_EVENTS)

	if (flags & RAINBOW_TOUCH_EVENTS)
		this->touch_subscribers.push_back(t);
}

void Input::touch_began(const Touch *const touches, const unsigned int count)
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_began(touches, count);
}

void Input::touch_canceled()
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_canceled();
}

void Input::touch_ended(const Touch *const touches, const unsigned int count)
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_ended(touches, count);
}

void Input::touch_moved(const Touch *const touches, const unsigned int count)
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_moved(touches, count);
}
