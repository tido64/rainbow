// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#include <utility>

void Input::subscribe(InputListener *const i)
{
	last_listener_->append(i);
	last_listener_ = i;
}

void Input::unsubscribe(InputListener *const i)
{
	i->pop();
}

void Input::accelerated(const double x,
                        const double y,
                        const double z,
                        const double t)
{
	acceleration_.update(x, y, z, t);
}

void Input::on_key_down(const Key &k)
{
	if (for_each(next(), [&k](InputListener *i) { return i->on_key_down(k); }))
		return;
}

void Input::on_key_up(const Key &k)
{
	if (for_each(next(), [&k](InputListener *i) { return i->on_key_up(k); }))
		return;
}

void Input::on_pointer_began(const unsigned int count, Pointer *pointers)
{
	auto began = [count, pointers](InputListener *i) {
		return i->on_pointer_began(count, pointers);
	};
	if (for_each(next(), std::move(began)))
		return;
}

void Input::on_pointer_canceled()
{
	auto canceled = [](InputListener *i) { return i->on_pointer_canceled(); };
	if (for_each(next(), std::move(canceled)))
		return;
}

void Input::on_pointer_ended(const unsigned int count, Pointer *pointers)
{
	auto ended = [count, pointers](InputListener *i) {
		return i->on_pointer_ended(count, pointers);
	};
	if (for_each(next(), std::move(ended)))
		return;
}

void Input::on_pointer_moved(const unsigned int count, Pointer *pointers)
{
	auto moved = [count, pointers](InputListener *i) {
		return i->on_pointer_moved(count, pointers);
	};
	if (for_each(next(), std::move(moved)))
		return;
}

void Input::on_end_link_removed(Link *node)
{
	last_listener_ = static_cast<InputListener*>(node)->prev();
}
