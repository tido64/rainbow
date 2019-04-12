// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUTLISTENER_H_
#define INPUT_INPUTLISTENER_H_

#include "Common/Link.h"
#include "Memory/Array.h"

namespace rainbow
{
    struct ControllerAxisMotion;
    struct ControllerButtonEvent;
    struct KeyStroke;
    struct Pointer;

    /// <summary>Interface for input listeners.</summary>
    class InputListener : private Link<InputListener>
    {
    public:
        /// <summary>
        ///   Returns the next input listener; <c>nullptr</c> if none.
        /// </summary>
        auto next() const { return Link::next(); }

        /// <summary>
        ///   Returns the previous input listener; <c>nullptr</c> if none.
        /// </summary>
        auto prev() const { return Link::prev(); }

        bool on_controller_axis_motion(const ControllerAxisMotion& motion)
        {
            return on_controller_axis_motion_impl(motion);
        }

        bool on_controller_button_down(const ControllerButtonEvent& button)
        {
            return on_controller_button_down_impl(button);
        }

        bool on_controller_button_up(const ControllerButtonEvent& button)
        {
            return on_controller_button_up_impl(button);
        }

        bool on_controller_connected(uint32_t port)
        {
            return on_controller_connected_impl(port);
        }

        bool on_controller_disconnected(uint32_t port)
        {
            return on_controller_disconnected_impl(port);
        }

        bool on_key_down(const KeyStroke& k) { return on_key_down_impl(k); }

        bool on_key_up(const KeyStroke& k) { return on_key_up_impl(k); }

        bool on_mouse_wheel(const ArrayView<Pointer>& wheels)
        {
            return on_mouse_wheel_impl(wheels);
        }

        /// <summary>User puts finger down on the device; mouse down.</summary>
        /// <param name="count">Number of pointers.</param>
        /// <param name="pointers">The id and position of each pointer.</param>
        bool on_pointer_began(const ArrayView<Pointer>& pointers)
        {
            return on_pointer_began_impl(pointers);
        }

        /// <summary>
        ///   Canceled pointer event. May occur when the user moves finger
        ///   off-screen or the application got interrupted by external events.
        /// <summary>
        bool on_pointer_canceled() { return on_pointer_canceled_impl(); }

        /// <summary>User lifts finger; mouse up.</summary>
        /// <param name="count">Number of pointers.</param>
        /// <param name="pointers">The id and position of each pointer.</param>
        bool on_pointer_ended(const ArrayView<Pointer>& pointers)
        {
            return on_pointer_ended_impl(pointers);
        }

        /// <summary>
        ///   User moves finger around while it still is on the device; mouse
        ///   moved.
        /// </summary>
        /// <param name="count">Number of pointers.</param>
        /// <param name="pointers">The id and position of each pointer.</param>
        bool on_pointer_moved(const ArrayView<Pointer>& pointers)
        {
            return on_pointer_moved_impl(pointers);
        }

    protected:
        ~InputListener() {}

    private:
        virtual bool on_controller_axis_motion_impl(const ControllerAxisMotion&)
        {
            return false;
        }

        virtual bool on_controller_button_down_impl(
            const ControllerButtonEvent&)
        {
            return false;
        }

        virtual bool on_controller_button_up_impl(const ControllerButtonEvent&)
        {
            return false;
        }

        virtual bool on_controller_connected_impl(uint32_t)
        {
            return false;
        }

        virtual bool on_controller_disconnected_impl(uint32_t)
        {
            return false;
        }

        virtual bool on_key_down_impl(const KeyStroke&) { return false; }
        virtual bool on_key_up_impl(const KeyStroke&) { return false; }

        virtual bool on_mouse_wheel_impl(const ArrayView<Pointer>&)
        {
            return false;
        }

        virtual bool on_pointer_began_impl(const ArrayView<Pointer>&)
        {
            return false;
        }

        virtual bool on_pointer_canceled_impl() { return false; }

        virtual bool on_pointer_ended_impl(const ArrayView<Pointer>&)
        {
            return false;
        }

        virtual bool on_pointer_moved_impl(const ArrayView<Pointer>&)
        {
            return false;
        }

        // Link implementation details

        void on_end_link_changed(InputListener& new_link) final;

        friend class Input;
        friend Link;
    };
}  // namespace rainbow

#endif
