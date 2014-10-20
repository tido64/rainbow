// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#include <utility>

#include "Input/Key.h"
#include "Lua/lua_Input.h"

void Input::subscribe(InputListener *const i)
{
	last_listener_->append(i);
	last_listener_ = i;
}

void Input::unsubscribe(InputListener *const i)
{
	i->pop();
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
void Input::on_key_down(const Key &k)
{
	if (for_each(next(), [&k](InputListener *i) { return i->on_key_down(k); }))
		return;

	Rainbow::Lua::Input::on_key_down(lua_state_, k);
}

void Input::on_key_up(const Key &k)
{
	if (for_each(next(), [&k](InputListener *i) { return i->on_key_up(k); }))
		return;

	Rainbow::Lua::Input::on_key_up(lua_state_, k);
}
#endif

void Input::on_touch_began(Touch *const touches, const size_t count)
{
	auto began = [touches, count](InputListener *i) {
		return i->on_touch_began(touches, count);
	};
	if (for_each(next(), std::move(began)))
		return;

	Rainbow::Lua::Input::on_touch_began(lua_state_, touches, count);
}

void Input::on_touch_canceled()
{
	auto canceled = [](InputListener *i) { return i->on_touch_canceled(); };
	if (for_each(next(), std::move(canceled)))
		return;

	Rainbow::Lua::Input::on_touch_canceled(lua_state_);
}

void Input::on_touch_ended(Touch *const touches, const size_t count)
{
	auto ended = [touches, count](InputListener *i) {
		return i->on_touch_ended(touches, count);
	};
	if (for_each(next(), std::move(ended)))
		return;

	Rainbow::Lua::Input::on_touch_ended(lua_state_, touches, count);
}

void Input::on_touch_moved(Touch *const touches, const size_t count)
{
	auto moved = [touches, count](InputListener *i) {
		return i->on_touch_moved(touches, count);
	};
	if (for_each(next(), std::move(moved)))
		return;

	Rainbow::Lua::Input::on_touch_moved(lua_state_, touches, count);
}

void Input::on_end_link_removed(Link *node)
{
	last_listener_ = static_cast<InputListener*>(node)->prev();
}
