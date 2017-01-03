// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_CONTROLLER_H_
#define INPUT_CONTROLLER_H_

#include <array>
#include <bitset>
#include <cstdint>
#include <memory>

#include "Common/Algorithm.h"
#include "Common/Logging.h"

namespace rainbow
{
    enum class ControllerAxis
    {
        Invalid = -1,
        LeftX,
        LeftY,
        RightX,
        RightY,
        TriggerLeft,
        TriggerRight,
        Count
    };

    enum class ControllerButton
    {
        Invalid = -1,
        A,
        B,
        X,
        Y,
        Back,
        Guide,
        Start,
        LeftStick,
        RightStick,
        LeftShoulder,
        RightShoulder,
        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,
        Count
    };

    struct ControllerAxisMotion
    {
        uint32_t id;
        ControllerAxis axis;
        int32_t value;
        uint64_t timestamp;

        ControllerAxisMotion()
            : ControllerAxisMotion(0, ControllerAxis::Invalid, 0, 0)
        {
        }

        ControllerAxisMotion(uint32_t id_,
                             ControllerAxis axis_,
                             int32_t value_,
                             uint64_t timestamp_)
            : id(id_), axis(axis_), value(value_), timestamp(timestamp_)
        {
        }
    };

    struct ControllerButtonEvent
    {
        uint32_t id;
        ControllerButton button;
        uint64_t timestamp;

        ControllerButtonEvent()
            : ControllerButtonEvent(0, ControllerButton::Invalid, 0)
        {
        }

        ControllerButtonEvent(uint32_t id_,
                              ControllerButton button_,
                              uint64_t timestamp_)
            : id(id_), button(button_), timestamp(timestamp_)
        {
        }
    };

    class ControllerState
    {
        static constexpr uint32_t kNoController =
            std::numeric_limits<uint32_t>::max();

    public:
        ControllerState() { unassign(); }

        auto id() const { return id_; }
        bool is_assigned() const { return id_ != kNoController; }

        void assign(uint32_t id) { id_ = id; }

        auto axis(ControllerAxis axis) const
        {
            return axes_[to_integral_value(axis)];
        }

        bool is_down(ControllerButton button) const
        {
            return buttons_[to_integral_value(button)];
        }

        void unassign()
        {
            id_ = kNoController;
            buttons_.reset();
            std::uninitialized_fill_n(axes_.data(), axes_.size(), 0);
        }

        void on_axis_motion(const ControllerAxisMotion& motion)
        {
            R_ASSERT(motion.id == id_,
                     "Controller axis motion event for wrong controller");

            axes_[to_integral_value(motion.axis)] = motion.value;
        }

        void on_button_down(const ControllerButtonEvent& event)
        {
            R_ASSERT(event.id == id_,
                     "Controller button event for wrong controller");

            buttons_.set(to_integral_value(event.button));
        }

        void on_button_up(const ControllerButtonEvent& event)
        {
            R_ASSERT(event.id == id_,
                     "Controller button event for wrong controller");

            buttons_.reset(to_integral_value(event.button));
        }

#ifdef RAINBOW_TEST
        auto buttons_down() const { return buttons_.count(); }
#endif  // RAINBOW_TEST

    private:
        uint32_t id_;
        std::bitset<to_integral_value(ControllerButton::Count)> buttons_;
        std::array<int, to_integral_value(ControllerAxis::Count)> axes_;
    };
}

#endif
