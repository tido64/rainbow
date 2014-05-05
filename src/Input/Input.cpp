// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#include "Input/Key.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

Input *Input::Instance = nullptr;

void Input::subscribe(Touchable *const t, const unsigned int events)
{
	//if (events & Events::Key)

	if (events & Events::Touch)
		this->touch_subscribers.push_back(t);
}

void Input::accelerated(const double x, const double y, const double z, const double t)
{
	this->acceleration.update(x, y, z, t);
	Rainbow::Lua::Input::accelerated(this->lua_state, this->acceleration);
}

void Input::clear()
{
	Rainbow::Lua::Input::clear(this->lua_state);
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
	for (auto subscriber : this->touch_subscribers)
		subscriber->touch_began(touches, count);
	Rainbow::Lua::Input::touch_began(this->lua_state, touches, count);
}

void Input::touch_canceled()
{
	for (auto subscriber : this->touch_subscribers)
		subscriber->touch_canceled();
	Rainbow::Lua::Input::touch_canceled(this->lua_state);
}

void Input::touch_ended(Touch *const touches, const size_t count)
{
	for (auto subscriber : this->touch_subscribers)
		subscriber->touch_ended(touches, count);
	Rainbow::Lua::Input::touch_ended(this->lua_state, touches, count);
}

void Input::touch_moved(Touch *const touches, const size_t count)
{
	for (auto subscriber : this->touch_subscribers)
		subscriber->touch_moved(touches, count);
	Rainbow::Lua::Input::touch_moved(this->lua_state, touches, count);
}
