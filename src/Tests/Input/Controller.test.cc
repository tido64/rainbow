// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/InputListener.h"

namespace
{
	class ControllerTestFixture : public InputListener
	{
	public:
		struct Events
		{
			static const unsigned int AxisMotion    = 1u << 0;
			static const unsigned int ButtonDown    = 1u << 1;
			static const unsigned int ButtonUp      = 1u << 2;
			static const unsigned int Connected     = 1u << 3;
			static const unsigned int Disconnected  = 1u << 4;
		};

		ControllerTestFixture() : flags_(0) {}

		ControllerAxisMotion axis_motion() const
		{
			return ControllerAxisMotion(id(), Controller::Axis::LeftX, 255, 1);
		}

		ControllerButton button() const
		{
			return ControllerButton(id(), Controller::Button::A, 1);
		}

		uint32_t id() const { return 1; }

		bool is_triggered(const unsigned int flags) { return flags_ & flags; }

	private:
		unsigned int flags_;

		bool on_controller_axis_motion_impl(
		    const ControllerAxisMotion& motion) override
		{
			const auto& expected = axis_motion();

			REQUIRE(motion.id == expected.id);
			REQUIRE(motion.axis == expected.axis);
			REQUIRE(motion.value == expected.value);
			REQUIRE(motion.timestamp == expected.timestamp);

			flags_ |= Events::AxisMotion;
			return true;
		}

		bool
		on_controller_button_down_impl(const ControllerButton& btn) override
		{
			const auto& expected = button();

			REQUIRE(btn.id == expected.id);
			REQUIRE(btn.button == expected.button);
			REQUIRE(btn.timestamp == expected.timestamp);

			flags_ |= Events::ButtonDown;
			return true;
		}

		bool
		on_controller_button_up_impl(const ControllerButton& btn) override
		{
			const auto& expected = button();

			REQUIRE(btn.id == expected.id);
			REQUIRE(btn.button == expected.button);
			REQUIRE(btn.timestamp == expected.timestamp);

			flags_ |= Events::ButtonUp;
			return true;
		}

		bool on_controller_connected_impl(const unsigned int i) override
		{
			REQUIRE(i == id());

			flags_ |= Events::Connected;
			return true;
		}

		bool on_controller_disconnected_impl(const unsigned int i) override
		{
			REQUIRE(i == id());

			flags_ |= Events::Disconnected;
			return true;
		}
	};
}

TEST_CASE("Controller input objects are invalid by default", "[input]")
{
	const ControllerAxisMotion axis;

	REQUIRE(axis.id == 0);
	REQUIRE(axis.axis == Controller::Axis::Invalid);
	REQUIRE(axis.value == 0);
	REQUIRE(axis.timestamp == 0);

	const ControllerButton button;

	REQUIRE(button.id == 0);
	REQUIRE(button.button == Controller::Button::Invalid);
	REQUIRE(button.timestamp == 0);
}

TEST_CASE("Controller events are propagated", "[input]")
{
	using ControllerEvents = ControllerTestFixture::Events;

	Input input;
	ControllerTestFixture delegate;
	input.subscribe(&delegate);

	SECTION("on_controller_axis_motion event")
	{
		input.on_controller_axis_motion(delegate.axis_motion());
		REQUIRE(delegate.is_triggered(ControllerEvents::AxisMotion));
		REQUIRE_FALSE(
		    delegate.is_triggered(0xff ^ ControllerEvents::AxisMotion));
	}

	SECTION("on_controller_button_down event")
	{
		input.on_controller_button_down(delegate.button());
		REQUIRE(delegate.is_triggered(ControllerEvents::ButtonDown));
		REQUIRE_FALSE(
		    delegate.is_triggered(0xff ^ ControllerEvents::ButtonDown));
	}

	SECTION("on_controller_button_up event")
	{
		input.on_controller_button_up(delegate.button());
		REQUIRE(delegate.is_triggered(ControllerEvents::ButtonUp));
		REQUIRE_FALSE(
		    delegate.is_triggered(0xff ^ ControllerEvents::ButtonUp));
	}

	SECTION("on_controller_connected event")
	{
		input.on_controller_connected(delegate.id());
		REQUIRE(delegate.is_triggered(ControllerEvents::Connected));
		REQUIRE_FALSE(
		    delegate.is_triggered(0xff ^ ControllerEvents::Connected));
	}

	SECTION("on_controller_disconnected event")
	{
		input.on_controller_disconnected(delegate.id());
		REQUIRE(delegate.is_triggered(ControllerEvents::Disconnected));
		REQUIRE_FALSE(
		    delegate.is_triggered(0xff ^ ControllerEvents::Disconnected));
	}
}
