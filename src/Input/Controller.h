// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_CONTROLLER_H_
#define INPUT_CONTROLLER_H_

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
            : id(0), axis(ControllerAxis::Invalid), value(0), timestamp(0) {}

        ControllerAxisMotion(uint32_t id,
                             ControllerAxis axis,
                             int32_t value,
                             uint64_t timestamp)
            : id(id), axis(axis), value(value), timestamp(timestamp) {}
    };

    struct ControllerButtonEvent
    {
        uint32_t id;
        ControllerButton button;
        uint64_t timestamp;

        ControllerButtonEvent()
            : id(0), button(ControllerButton::Invalid), timestamp(0) {}

        ControllerButtonEvent(uint32_t id,
                              ControllerButton button,
                              uint64_t timestamp)
            : id(id), button(button), timestamp(timestamp) {}
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
            return axes_[to_underlying_type(axis)];
        }

        bool is_down(ControllerButton button) const
        {
            return buttons_[to_underlying_type(button)];
        }

        void unassign()
        {
            id_ = kNoController;
            std::uninitialized_fill_n(axes_, array_size(axes_), 0);
            buttons_.reset();
        }

        void on_axis_motion(const ControllerAxisMotion& motion)
        {
            R_ASSERT(motion.id == id_,
                     "Controller axis motion event for wrong controller");

            axes_[to_underlying_type(motion.axis)] = motion.value;
        }

        void on_button_down(const ControllerButtonEvent& event)
        {
            R_ASSERT(event.id == id_,
                     "Controller button event for wrong controller");

            buttons_.set(to_underlying_type(event.button));
        }

        void on_button_up(const ControllerButtonEvent& event)
        {
            R_ASSERT(event.id == id_,
                     "Controller button event for wrong controller");

            buttons_.reset(to_underlying_type(event.button));
        }

#ifdef RAINBOW_TEST
        auto buttons_down() const { return buttons_.count(); }
#endif  // RAINBOW_TEST

    private:
        uint32_t id_;
        int axes_[to_underlying_type(ControllerAxis::Count)];
        std::bitset<to_underlying_type(ControllerButton::Count)> buttons_;
    };
}

#endif
