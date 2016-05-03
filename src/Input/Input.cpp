// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

using rainbow::ControllerAxisMotion;
using rainbow::ControllerButtonEvent;
using rainbow::KeyMods;
using rainbow::KeyStroke;
using rainbow::VirtualKey;

namespace
{
    template <typename T, typename F>
    void flip_keys(const KeyStroke& stroke, T&& keys, F&& flip)
    {
        flip(keys, static_cast<unsigned>(stroke.key));
        if (stroke.mods != KeyMods::None)
        {
            if ((stroke.mods & KeyMods::LeftShift) == KeyMods::LeftShift)
                flip(keys, static_cast<unsigned>(VirtualKey::LeftShift));
            if ((stroke.mods & KeyMods::RightShift) == KeyMods::RightShift)
                flip(keys, static_cast<unsigned>(VirtualKey::RightShift));
            if ((stroke.mods & KeyMods::LeftCtrl) == KeyMods::LeftCtrl)
                flip(keys, static_cast<unsigned>(VirtualKey::LeftCtrl));
            if ((stroke.mods & KeyMods::RightCtrl) == KeyMods::RightCtrl)
                flip(keys, static_cast<unsigned>(VirtualKey::RightCtrl));
            if ((stroke.mods & KeyMods::LeftAlt) == KeyMods::LeftAlt)
                flip(keys, static_cast<unsigned>(VirtualKey::LeftAlt));
            if ((stroke.mods & KeyMods::RightAlt) == KeyMods::RightAlt)
                flip(keys, static_cast<unsigned>(VirtualKey::RightAlt));
            if ((stroke.mods & KeyMods::LeftSuper) == KeyMods::LeftSuper)
                flip(keys, static_cast<unsigned>(VirtualKey::LeftSuper));
            if ((stroke.mods & KeyMods::RightSuper) == KeyMods::RightSuper)
                flip(keys, static_cast<unsigned>(VirtualKey::RightSuper));
        }
    }
}

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

void Input::on_controller_button_down(const ControllerButtonEvent& button)
{
    for_each(next(), [&button](InputListener* i) {
        return i->on_controller_button_down(button);
    });
}

void Input::on_controller_button_up(const ControllerButtonEvent& button)
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

void Input::on_key_down(const KeyStroke& k)
{
    flip_keys(k, keys_, [](decltype(keys_)& keys, unsigned pos) {
        keys.set(pos);
    });

    for_each(next(), [&k](InputListener* i) { return i->on_key_down(k); });
}

void Input::on_key_up(const KeyStroke& k)
{
    flip_keys(k, keys_, [](decltype(keys_)& keys, unsigned pos) {
        keys.reset(pos);
    });

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
