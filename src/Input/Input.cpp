// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#include "Input/Key.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

void Input::subscribe(Touchable *const t, const unsigned int events)
{
	//if (events & Events::Key)

	if (events & Events::Touch)
		touch_subscribers_.push_back(t);
}

void Input::accelerated(const double x, const double y, const double z, const double t)
{
	acceleration_.update(x, y, z, t);
	Rainbow::Lua::Input::accelerated(lua_state_, acceleration_);
}

void Input::clear()
{
	Rainbow::Lua::Input::clear(lua_state_);
}

#ifdef RAINBOW_BUTTONS

void Input::key_down(const Key &k)
{
	Rainbow::Lua::Input::key_down(lua_state_, k);
}

void Input::key_up(const Key &k)
{
	Rainbow::Lua::Input::key_up(lua_state_, k);
}

#endif

void Input::touch_began(Touch *const touches, const size_t count)
{
	for (auto subscriber : touch_subscribers_)
		subscriber->touch_began(touches, count);
	Rainbow::Lua::Input::touch_began(lua_state_, touches, count);
}

void Input::touch_canceled()
{
	for (auto subscriber : touch_subscribers_)
		subscriber->touch_canceled();
	Rainbow::Lua::Input::touch_canceled(lua_state_);
}

void Input::touch_ended(Touch *const touches, const size_t count)
{
	for (auto subscriber : touch_subscribers_)
		subscriber->touch_ended(touches, count);
	Rainbow::Lua::Input::touch_ended(lua_state_, touches, count);
}

void Input::touch_moved(Touch *const touches, const size_t count)
{
	for (auto subscriber : touch_subscribers_)
		subscriber->touch_moved(touches, count);
	Rainbow::Lua::Input::touch_moved(lua_state_, touches, count);
}
