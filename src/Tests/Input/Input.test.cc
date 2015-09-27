// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Input/Key.h"

namespace
{
	class DefaultInputListener : public InputListener {};

	class TestInputListener : public InputListener
	{
	public:
		TestInputListener(const bool consume)
		    : consume_(consume), axis_motion_(false), button_down_(false),
		      button_up_(false), controller_connected_(false),
		      controller_disconnected_(false), key_down_(false), key_up_(false),
		      pointer_began_(false), pointer_canceled_(false),
		      pointer_ended_(false), pointer_moved_(false) {}

		bool axis_motion() const { return axis_motion_; }
		bool button_down() const { return button_down_; }
		bool button_up() const { return button_up_; }
		bool controller_connected() const { return controller_connected_; }

		bool controller_disconnected() const
		{
			return controller_disconnected_;
		}

		bool key_down() const { return key_down_; }
		bool key_up() const { return key_up_; }
		bool pointer_began() const { return pointer_began_; }
		bool pointer_canceled() const { return pointer_canceled_; }
		bool pointer_ended() const { return pointer_ended_; }
		bool pointer_moved() const { return pointer_moved_; }

		void reset() { pointer_began_ = false; }

		explicit operator bool() const { return pointer_began_; }

	private:
		const bool consume_;
		bool axis_motion_;
		bool button_down_;
		bool button_up_;
		bool controller_connected_;
		bool controller_disconnected_;
		bool key_down_;
		bool key_up_;
		bool pointer_began_;
		bool pointer_canceled_;
		bool pointer_ended_;
		bool pointer_moved_;

		bool
		on_controller_axis_motion_impl(const ControllerAxisMotion&) override
		{
			axis_motion_ = true;
			return consume_;
		}

		bool on_controller_button_down_impl(const ControllerButton&) override
		{
			button_down_ = true;
			return consume_;
		}

		bool on_controller_button_up_impl(const ControllerButton&) override
		{
			button_up_ = true;
			return consume_;
		}

		bool on_controller_connected_impl(const unsigned int) override
		{
			controller_connected_ = true;
			return consume_;
		}

		bool on_controller_disconnected_impl(const unsigned int) override
		{
			controller_disconnected_ = true;
			return consume_;
		}

		bool on_key_down_impl(const Key&) override
		{
			key_down_ = true;
			return consume_;
		}

		bool on_key_up_impl(const Key&) override
		{
			key_up_ = true;
			return consume_;
		}

		bool on_pointer_began_impl(const unsigned int, const Pointer*) override
		{
			pointer_began_ = true;
			return consume_;
		}

		bool on_pointer_canceled_impl() override
		{
			pointer_canceled_ = true;
			return consume_;
		}

		bool on_pointer_ended_impl(const unsigned int, const Pointer*) override
		{
			pointer_ended_ = true;
			return consume_;
		}

		bool on_pointer_moved_impl(const unsigned int, const Pointer*) override
		{
			pointer_moved_ = true;
			return consume_;
		}
	};
}

TEST_CASE("Default input listener implementation does nothing", "[input]")
{
	DefaultInputListener default_input_listener;
	TestInputListener test_input_listener(false);
	Input input;
	input.subscribe(&default_input_listener);
	input.subscribe(&test_input_listener);

	REQUIRE_FALSE(test_input_listener.axis_motion());
	input.on_controller_axis_motion(ControllerAxisMotion());
	REQUIRE(test_input_listener.axis_motion());

	REQUIRE_FALSE(test_input_listener.button_down());
	input.on_controller_button_down(ControllerButton());
	REQUIRE(test_input_listener.button_down());

	REQUIRE_FALSE(test_input_listener.button_up());
	input.on_controller_button_up(ControllerButton());
	REQUIRE(test_input_listener.button_up());

	REQUIRE_FALSE(test_input_listener.controller_connected());
	input.on_controller_connected(0);
	REQUIRE(test_input_listener.controller_connected());

	REQUIRE_FALSE(test_input_listener.controller_disconnected());
	input.on_controller_disconnected(0);
	REQUIRE(test_input_listener.controller_disconnected());

	REQUIRE_FALSE(test_input_listener.key_down());
	input.on_key_down(Key());
	REQUIRE(test_input_listener.key_down());

	REQUIRE_FALSE(test_input_listener.key_up());
	input.on_key_up(Key());
	REQUIRE(test_input_listener.key_up());

	REQUIRE_FALSE(test_input_listener.pointer_began());
	input.on_pointer_began(0, nullptr);
	REQUIRE(test_input_listener.pointer_began());

	REQUIRE_FALSE(test_input_listener.pointer_canceled());
	input.on_pointer_canceled();
	REQUIRE(test_input_listener.pointer_canceled());

	REQUIRE_FALSE(test_input_listener.pointer_ended());
	input.on_pointer_ended(0, nullptr);
	REQUIRE(test_input_listener.pointer_ended());

	REQUIRE_FALSE(test_input_listener.pointer_moved());
	input.on_pointer_moved(0, nullptr);
	REQUIRE(test_input_listener.pointer_moved());
}

TEST_CASE("Input listeners can prevent further propagation", "[input]")
{
	Input input;
	TestInputListener listeners[]{TestInputListener{true},
	                              TestInputListener{false}};
	input.subscribe(listeners);
	input.subscribe(listeners + 1);
	input.on_pointer_began(0, nullptr);

	REQUIRE(listeners[0]);
	REQUIRE_FALSE(listeners[1]);
}

TEST_CASE("Input listeners can unsubscribe", "[input]")
{
	Input input;
	TestInputListener listeners[]{TestInputListener{false},
	                              TestInputListener{true},
	                              TestInputListener{true}};
	for (auto& i : listeners)
		input.subscribe(&i);
	input.on_pointer_began(0, nullptr);

	REQUIRE(listeners[0]);
	REQUIRE(listeners[1]);
	REQUIRE_FALSE(listeners[2]);

	for (auto& i : listeners)
		i.reset();
	input.unsubscribe(listeners + 1);
	input.on_pointer_began(0, nullptr);

	REQUIRE(listeners[0]);
	REQUIRE_FALSE(listeners[1]);
	REQUIRE(listeners[2]);
}

TEST_CASE("Input listeners are unsubscribed when deleted", "[input]")
{
	Input input;
	TestInputListener l{true};
	{
		TestInputListener listeners[]{TestInputListener{false},
		                              TestInputListener{false},
		                              TestInputListener{false}};
		for (auto& i : listeners)
			input.subscribe(&i);
		input.subscribe(&l);
		input.on_pointer_began(0, nullptr);
		for (auto& i : listeners)
		{
			REQUIRE(i);
			i.reset();
		}
	}
	input.on_pointer_began(0, nullptr);
	REQUIRE(l);
}

TEST_CASE("Input manager is notified of popped end links", "[input]")
{
	Input input;
	TestInputListener a{true};
	{
		TestInputListener b{false};
		input.subscribe(&b);
		{
			TestInputListener c{true};
			input.subscribe(&c);
			input.on_pointer_began(0, nullptr);
			REQUIRE(b);
			REQUIRE(c);
			b.reset();
		}
		input.subscribe(&a);
		input.on_pointer_began(0, nullptr);
		REQUIRE(b);
		REQUIRE(a);
	}
	input.unsubscribe(&a);
	a.reset();
	input.subscribe(&a);
	input.on_pointer_began(0, nullptr);
	REQUIRE(a);
}
