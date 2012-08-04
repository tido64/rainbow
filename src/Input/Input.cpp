// Copyright 2010-12 Bifrost Entertainment. All rights reserved.

#include "Input/Input.h"
#include "Input/Key.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

void Input::subscribe(Touchable *const t, unsigned int flags)
{
	//if (flags & RAINBOW_KEY_EVENTS)

	if (flags & RAINBOW_TOUCH_EVENTS)
		this->touch_subscribers.push_back(t);
}

void Input::accelerated(const double x, const double y, const double z, const double t)
{
	this->acceleration.update(x, y, z, t);
	Rainbow::Lua::Input::accelerated(this->lua_state, this->acceleration);
}

#ifdef RAINBOW_BUTTONS

void Input::key_down(const Key &k)
{
	Rainbow::Lua::Input::key_down(this->lua_state, k);
}

void Input::key_up(const Key &k)
{
	Rainbow::Lua::Input::key_up(this->lua_state, k);
}

#endif

void Input::touch_began(Touch *const touches, const size_t count)
{
	for (size_t i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_began(touches, count);
	Rainbow::Lua::Input::touch_began(this->lua_state, touches, count);
}

void Input::touch_canceled()
{
	for (size_t i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_canceled();
	Rainbow::Lua::Input::touch_canceled(this->lua_state);
}

void Input::touch_ended(Touch *const touches, const size_t count)
{
	for (size_t i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_ended(touches, count);
	Rainbow::Lua::Input::touch_ended(this->lua_state, touches, count);
}

void Input::touch_moved(Touch *const touches, const size_t count)
{
	for (size_t i = 0; i < this->touch_subscribers.size(); ++i)
		this->touch_subscribers[i]->touch_moved(touches, count);
	Rainbow::Lua::Input::touch_moved(this->lua_state, touches, count);
}
