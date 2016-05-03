// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_CONTROLLER_H_
#define INPUT_CONTROLLER_H_

#include <cstdint>

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
}

#endif
