// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Input/Key.h"
#include "Input/Input.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

void Input::subscribe(Touchable *const t, unsigned int flags)
{
	//if (flags & RAINBOW_KEY_EVENTS)

	if (flags & RAINBOW_TOUCH_EVENTS)
		this->touch_subscribers.push_back(t);
}

#ifdef RAINBOW_BUTTONS

void Input::key_down(const Key &k)
{
	lua_Input::key_down(this->lua_state, k);
}

void Input::key_up(const Key &k)
{
	lua_Input::key_up(this->lua_state, k);
}

#endif

void Input::touch_began(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_began(touches, count);
	lua_Input::touch_began(this->lua_state, touches, count);
}

void Input::touch_canceled()
{
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_canceled();
	lua_Input::touch_canceled(this->lua_state);
}

void Input::touch_ended(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_ended(touches, count);
	lua_Input::touch_ended(this->lua_state, touches, count);
}

void Input::touch_moved(Touch *const touches, const unsigned int count)
{
	this->touch_flip(touches, count);
	for (unsigned int i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_moved(touches, count);
	lua_Input::touch_moved(this->lua_state, touches, count);
}

void Input::touch_flip(Touch *const touches, const unsigned int count)
{
#ifdef RAINBOW_IOS
	const UIInterfaceOrientation orientation = [[UIApplication sharedApplication] statusBarOrientation];
	if (UIInterfaceOrientationIsLandscape(orientation))
	{
		int tmp = 0;
		for (unsigned int i = 0; i < count; ++i)
		{
			tmp = touches[i].x;
			touches[i].x = touches[i].y;
			touches[i].y = tmp;
		}
		return;
	}
#endif

	for (unsigned int i = 0; i < count; ++i)
		touches[i].y = this->height - touches[i].y;
}
