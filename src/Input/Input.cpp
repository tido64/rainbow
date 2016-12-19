// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Input/Input.h"

#define kInvalidControllerAxis    "Invalid controller axis"
#define kInvalidControllerButton  "Invalid controller button"
#define kInvalidVirtualKey        "Invalid virtual key"

using rainbow::ControllerAxisMotion;
using rainbow::ControllerButtonEvent;
using rainbow::Input;
using rainbow::KeyMods;
using rainbow::KeyStroke;
using rainbow::Pointer;
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

constexpr unsigned int Input::kNumSupportedControllers;

template <typename F>
void Input::process_controller(unsigned int id, F&& process)
{
    for (auto i = 0u; i < array_size(controllers_); ++i)
    {
        if (controllers_[i].id() == id)
        {
            process(i);
            break;
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

void Input::on_controller_axis_motion(const ControllerAxisMotion& motion)
{
    R_ASSERT(static_cast<int>(motion.axis) >= 0 &&
                 motion.axis < ControllerAxis::Count,
             kInvalidControllerAxis);

    process_controller(motion.id, [this, motion](unsigned int i) {
        controllers_[i].on_axis_motion(motion);
        auto event = motion;
        event.id = i;
        for_each(next(), [&event](InputListener* i) {
            return i->on_controller_axis_motion(event);
        });
    });
}

void Input::on_controller_button_down(const ControllerButtonEvent& button)
{
    R_ASSERT(static_cast<int>(button.button) >= 0 &&
                 button.button < ControllerButton::Count,
             kInvalidControllerButton);

    process_controller(button.id, [this, button](unsigned int i) {
        controllers_[i].on_button_down(button);
        auto event = button;
        event.id = i;
        for_each(next(), [&event](InputListener* i) {
            return i->on_controller_button_down(event);
        });
    });
}

void Input::on_controller_button_up(const ControllerButtonEvent& button)
{
    R_ASSERT(static_cast<int>(button.button) >= 0 &&
                 button.button < ControllerButton::Count,
             kInvalidControllerButton);

    process_controller(button.id, [this, button](unsigned int i) {
        controllers_[i].on_button_up(button);
        auto event = button;
        event.id = i;
        for_each(next(), [&event](InputListener* i) {
            return i->on_controller_button_up(event);
        });
    });
}

void Input::on_controller_connected(unsigned int id)
{
    int port = -1;
    for (auto i = 0u; i < array_size(controllers_); ++i)
    {
        if (controllers_[i].id() == id)
            return;

        if (port < 0 && !controllers_[i].is_assigned())
            port = static_cast<int>(i);
    }

    if (port < 0)
        return;

    controllers_[port].assign(id);
    LOGI("Controller %u plugged into port %d", id, port + 1);

    for_each(next(), [port](InputListener* i) {
        return i->on_controller_connected(port);
    });
}

void Input::on_controller_disconnected(unsigned int id)
{
    process_controller(id, [this](unsigned int i) {
        auto& controller = controllers_[i];
        LOGI("Controller %u unplugged from port %d", controller.id(), i + 1);
        controller.unassign();

        for_each(next(), [i](InputListener* listener) {
            return listener->on_controller_disconnected(i);
        });
    });
}

void Input::on_key_down(const KeyStroke& k)
{
    R_ASSERT(static_cast<int>(k.key) >= 0 && k.key < VirtualKey::KeyCount,
             kInvalidVirtualKey);

    flip_keys(k, keys_, [](decltype(keys_)& keys, unsigned pos) {
        keys.set(pos);
    });

    for_each(next(), [&k](InputListener* i) { return i->on_key_down(k); });
}

void Input::on_key_up(const KeyStroke& k)
{
    R_ASSERT(static_cast<int>(k.key) >= 0 && k.key < VirtualKey::KeyCount,
             kInvalidVirtualKey);

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
