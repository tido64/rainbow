// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_CONTROLLER_H_
#define INPUT_CONTROLLER_H_

#include <cstdint>

namespace Controller
{
	enum class Axis
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

	enum class Button
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
}

struct ControllerAxisMotion
{
	uint32_t id;
	Controller::Axis axis;
	int32_t value;
	uint64_t timestamp;

	ControllerAxisMotion()
	    : id(0), axis(Controller::Axis::Invalid), value(0), timestamp(0) {}

	ControllerAxisMotion(const uint32_t id,
	                     const Controller::Axis axis,
	                     const int32_t value,
	                     const uint64_t timestamp)
	    : id(id), axis(axis), value(value), timestamp(timestamp) {}
};

struct ControllerButton
{
	uint32_t id;
	Controller::Button button;
	uint64_t timestamp;

	ControllerButton()
	    : id(0), button(Controller::Button::Invalid), timestamp(0) {}

	ControllerButton(const uint32_t id,
	                 const Controller::Button button,
	                 const uint64_t timestamp)
	    : id(id), button(button), timestamp(timestamp) {}
};

#endif
