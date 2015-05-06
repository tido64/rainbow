// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Input/Input.h"
#include "Input/InputListener.h"

namespace
{
	class TestInputListener : public InputListener
	{
	public:
		TestInputListener(const bool consume)
		    : consume_(consume), touched_(false) {}

		void reset() { touched_ = false; }

		explicit operator bool() const { return touched_; }

	private:
		const bool consume_;
		bool touched_;

		bool on_pointer_began_impl(const unsigned int,
		                           const Pointer *) override
		{
			touched_ = true;
			return consume_;
		}
	};
}

TEST_CASE("Input listeners can prevent further propagation", "[input]")
{
	Input input;
	TestInputListener listeners[] {
	    TestInputListener{true},
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
	TestInputListener listeners[] {
	    TestInputListener{false},
	    TestInputListener{true},
	    TestInputListener{true}};
	for (auto &i : listeners)
		input.subscribe(&i);
	input.on_pointer_began(0, nullptr);
	REQUIRE(listeners[0]);
	REQUIRE(listeners[1]);
	REQUIRE_FALSE(listeners[2]);
	for (auto &i : listeners)
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
		TestInputListener listeners[] {
		    TestInputListener{false},
		    TestInputListener{false},
		    TestInputListener{false}};
		for (auto &i : listeners)
			input.subscribe(&i);
		input.subscribe(&l);
		input.on_pointer_began(0, nullptr);
		for (auto &i : listeners)
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
