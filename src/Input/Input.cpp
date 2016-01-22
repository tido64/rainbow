// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

void Input::subscribe(NotNull<InputListener*> i)
{
    last_listener_->append(i);
    last_listener_ = i;
}

void Input::unsubscribe(NotNull<InputListener*> i)
{
    i->pop();
}

void Input::accelerated(double x, double y, double z, double t)
{
    acceleration_.update(x, y, z, t);
}

void Input::on_controller_axis_motion(const ControllerAxisMotion& axis_motion)
{
    for_each(next(), [&axis_motion](InputListener* i) {
        return i->on_controller_axis_motion(axis_motion);
    });
}

void Input::on_controller_button_down(const ControllerButton& button)
{
    for_each(next(), [&button](InputListener* i) {
        return i->on_controller_button_down(button);
    });
}

void Input::on_controller_button_up(const ControllerButton& button)
{
    for_each(next(), [&button](InputListener* i) {
        return i->on_controller_button_up(button);
    });
}

void Input::on_controller_connected(unsigned int id)
{
    for_each(next(), [id](InputListener* i) {
        return i->on_controller_connected(id);
    });
}

void Input::on_controller_disconnected(unsigned int id)
{
    for_each(next(), [id](InputListener* i) {
        return i->on_controller_disconnected(id);
    });
}

void Input::on_key_down(const Key& k)
{
    for_each(next(), [&k](InputListener* i) { return i->on_key_down(k); });
}

void Input::on_key_up(const Key& k)
{
    for_each(next(), [&k](InputListener* i) { return i->on_key_up(k); });
}

void Input::on_pointer_began(const ArrayView<Pointer>& pointers)
{
    for_each(next(), [&pointers](InputListener* i) {
        return i->on_pointer_began(pointers);
    });
}

void Input::on_pointer_canceled()
{
    for_each(next(), [](InputListener* i) { return i->on_pointer_canceled(); });
}

void Input::on_pointer_ended(const ArrayView<Pointer>& pointers)
{
    for_each(next(), [&pointers](InputListener* i) {
        return i->on_pointer_ended(pointers);
    });
}

void Input::on_pointer_moved(const ArrayView<Pointer>& pointers)
{
    for_each(next(), [&pointers](InputListener* i) {
        return i->on_pointer_moved(pointers);
    });
}

void Input::on_end_link_removed(Link* node)
{
    last_listener_ = static_cast<InputListener*>(node)->prev();
}
