// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_INPUT_H_
#define INPUT_INPUT_H_

#include "Common/Global.h"
#include "Common/Passkey.h"
#include "Input/Acceleration.h"
#include "Input/Controller.h"
#include "Input/InputListener.h"
#include "Input/VirtualKey.h"

namespace rainbow
{
    /// <summary>
    ///   Handles accelerometer/gyroscope and pointer events independent of
    ///   platform.
    /// <summary>
    /// <remarks>
    ///   <para>
    ///     Accelerometer data changes do not follow the observer pattern
    ///     because they happen every frame. Polling is preferred in this case.
    ///   </para>
    ///   References
    ///   <list type="bullet">
    ///     <item>http://developer.apple.com/library/ios/#documentation/UIKit/Reference/UIAcceleration_Class/Reference/UIAcceleration.html</item>
    ///   </list>
    /// <remarks>
    class Input : public Global<Input>, private InputListener
    {
    public:
        static constexpr unsigned int kNumSupportedControllers = 4;

        Input() : keys_({}), last_listener_(this) { make_global(); }

        /// <summary>
        ///   Returns the value of the specified axis on specified controller.
        /// </summary>
        auto axis(unsigned int controller, ControllerAxis axis) const
        {
            return controllers_[controller].axis(axis);
        }

        /// <summary>
        ///   Returns whether specified button is currently pressed on specified
        ///   controller.
        /// </summary>
        bool is_down(unsigned int controller, ControllerButton btn) const
        {
            return controllers_[controller].is_down(btn);
        }

        /// <summary>
        ///   Returns whether specified key is currently pressed.
        /// </summary>
        bool is_down(VirtualKey key) const
        {
            return keys_[to_integral_value(key)];
        }

        /// <summary>Clears all input listeners.</summary>
        void reset()
        {
            pop();
            last_listener_ = this;
        }

        /// <summary>Subscribes an object to input events.</summary>
        /// <param name="listener">The object to subscribe.</param>
        void subscribe(InputListener& listener);

        /// <summary>Unsubscribes an object from input events.</summary>
        /// <param name="listener">The object to unsubscribe.</param>
        void unsubscribe(InputListener& listener);

        /// <summary>Acceleration event.</summary>
        /// <param name="x">Acceleration data (x-value).</param>
        /// <param name="y">Acceleration data (y-value).</param>
        /// <param name="z">Acceleration data (z-value).</param>
        /// <param name="t">
        ///   The relative time at which the acceleration occurred.
        /// </param>
        void accelerated(double x, double y, double z, double t);

        void on_controller_axis_motion(const ControllerAxisMotion&);
        void on_controller_button_down(const ControllerButtonEvent&);
        void on_controller_button_up(const ControllerButtonEvent&);
        void on_controller_connected(unsigned int id);
        void on_controller_disconnected(unsigned int id);

        void on_key_down(const KeyStroke&);
        void on_key_up(const KeyStroke&);

        void on_last_listener_changed(InputListener& new_listener,
                                      Passkey<InputListener>);

        void on_pointer_began(const ArrayView<Pointer>& pointers);
        void on_pointer_canceled();
        void on_pointer_ended(const ArrayView<Pointer>& pointers);
        void on_pointer_moved(const ArrayView<Pointer>& pointers);

#ifdef RAINBOW_TEST
        auto buttons_down(unsigned int i) const
        {
            return controllers_[i].buttons_down();
        }

        auto connected_controllers() const
        {
            return std::count_if(
                std::cbegin(controllers_),
                std::cend(controllers_),
                [](auto&& controller) { return controller.is_assigned(); });
        }

        auto keys_down() const
        {
            return std::count(keys_.cbegin(), keys_.cend(), true);
        }
#endif  // RAINBOW_TEST

    private:
        std::array<ControllerState, kNumSupportedControllers> controllers_;
        std::array<bool, to_integral_value(VirtualKey::KeyCount)> keys_;
        Acceleration acceleration_;  ///< Accelerometer data
        InputListener* last_listener_;

        template <typename F>
        void process_controller(unsigned int id, F&& process);
    };
}

#endif
