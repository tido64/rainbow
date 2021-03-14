// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_CONTROLLER_H_
#define INPUT_CONTROLLER_H_

#include <array>
#include <cstdint>
#include <memory>

#ifdef RAINBOW_TEST
#    include <algorithm>
#endif

#include "Common/Logging.h"
#include "Common/TypeCast.h"

namespace rainbow
{
    using ControllerID = int32_t;

    enum class ControllerAxis {
        Invalid = -1,
        LeftX,
        LeftY,
        RightX,
        RightY,
        TriggerLeft,
        TriggerRight,
        Count
    };

    enum class ControllerButton {
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

    struct ControllerAxisMotion {
        ControllerID id;
        ControllerAxis axis;
        int32_t value;
        uint64_t timestamp;

        ControllerAxisMotion()
            : ControllerAxisMotion(0, ControllerAxis::Invalid, 0, 0)
        {
        }

        ControllerAxisMotion(ControllerID id_,
                             ControllerAxis axis_,
                             int32_t value_,
                             uint64_t timestamp_)
            : id(id_), axis(axis_), value(value_), timestamp(timestamp_)
        {
        }
    };

    struct ControllerButtonEvent {
        ControllerID id;
        ControllerButton button;
        uint64_t timestamp;

        ControllerButtonEvent()
            : ControllerButtonEvent(0, ControllerButton::Invalid, 0)
        {
        }

        ControllerButtonEvent(ControllerID id_,
                              ControllerButton button_,
                              uint64_t timestamp_)
            : id(id_), button(button_), timestamp(timestamp_)
        {
        }
    };

    class ControllerState
    {
        using AxisStates =
            std::array<int, to_underlying_type(ControllerAxis::Count)>;
        using ButtonStates =
            std::array<bool, to_underlying_type(ControllerButton::Count)>;

        static constexpr ControllerID kNoController = -1;

    public:
        ControllerState() : id_(kNoController), buttons_{}, axes_{} {}

        [[nodiscard]] auto axes() const -> const AxisStates& { return axes_; }

        [[nodiscard]] auto buttons() const -> const ButtonStates&
        {
            return buttons_;
        }

        [[nodiscard]] auto id() const { return id_; }
        [[nodiscard]] auto is_assigned() const { return id_ != kNoController; }

        void assign(ControllerID id) { id_ = id; }

        [[nodiscard]] auto axis(ControllerAxis axis) const
        {
            return axes_[to_underlying_type(axis)];
        }

        [[nodiscard]] auto is_down(ControllerButton button) const
        {
            return buttons_[to_underlying_type(button)];
        }

        void unassign()
        {
            id_ = kNoController;
            buttons_.fill(false);
            axes_.fill(0);
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

            buttons_[to_underlying_type(event.button)] = true;
        }

        void on_button_up(const ControllerButtonEvent& event)
        {
            R_ASSERT(event.id == id_,
                     "Controller button event for wrong controller");

            buttons_[to_underlying_type(event.button)] = false;
        }

#ifdef RAINBOW_TEST
        [[nodiscard]] auto buttons_down() const
        {
            return std::count(buttons_.cbegin(), buttons_.cend(), true);
        }
#endif  // RAINBOW_TEST

    private:
        ControllerID id_;
        ButtonStates buttons_;
        AxisStates axes_;
    };
}  // namespace rainbow

#endif
