// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <catch.hpp>

#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Input/Pointer.h"

namespace
{
	class PointerTestFixture : public InputListener
	{
	public:
		struct Events
		{
			static const unsigned int Began     = 1u << 0;
			static const unsigned int Canceled  = 1u << 1;
			static const unsigned int Ended     = 1u << 2;
			static const unsigned int Moved     = 1u << 3;
		};

		PointerTestFixture() : flags_(0) {}

		bool is_invalid(const unsigned int count, const Pointer *pointers)
		{
			return count != 1
			    || pointers[0].hash != 1
			    || pointers[0].x != 2
			    || pointers[0].y != 3
			    || pointers[0].timestamp != 0;
		}

		bool is_triggered(const unsigned int flags)
		{
			return flags_ & flags;
		}

	private:
		unsigned int flags_;

		bool on_pointer_began_impl(const unsigned int count,
		                           const Pointer *pointers) override
		{
			if (is_invalid(count, pointers))
				return false;

			flags_ |= Events::Began;
			return true;
		}

		bool on_pointer_canceled_impl() override
		{
			flags_ |= Events::Canceled;
			return true;
		}

		bool on_pointer_ended_impl(const unsigned int count,
		                           const Pointer *pointers) override
		{
			if (is_invalid(count, pointers))
				return false;

			flags_ |= Events::Ended;
			return true;
		}

		bool on_pointer_moved_impl(const unsigned int count,
		                           const Pointer *pointers) override
		{
			if (is_invalid(count, pointers))
				return false;

			flags_ |= Events::Moved;
			return true;
		}
	};
}

TEST_CASE("Pointer is all zero by default", "[input]")
{
	const Pointer p;

	REQUIRE(p.hash == 0);
	REQUIRE(p.x == 0);
	REQUIRE(p.y == 0);
	REQUIRE(p.timestamp == 0);
}

TEST_CASE("Pointer events", "[input]")
{
	using PointerEvents = PointerTestFixture::Events;

	Input input;
	PointerTestFixture delegate;
	input.subscribe(&delegate);

	SECTION("pointer_began event")
	{
		Pointer p1(1, 2, 3, 0);
		input.on_pointer_began(1, &p1);
		REQUIRE(delegate.is_triggered(PointerEvents::Began));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ PointerEvents::Began));
	}

	SECTION("pointer_canceled event")
	{
		input.on_pointer_canceled();
		REQUIRE(delegate.is_triggered(PointerEvents::Canceled));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ PointerEvents::Canceled));
	}

	SECTION("pointer_ended event")
	{
		Pointer p1(1, 2, 3, 0);
		input.on_pointer_ended(1, &p1);
		REQUIRE(delegate.is_triggered(PointerEvents::Ended));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ PointerEvents::Ended));
	}

	SECTION("pointer_moved event")
	{
		Pointer p1(1, 2, 3, 0);
		input.on_pointer_moved(1, &p1);
		REQUIRE(delegate.is_triggered(PointerEvents::Moved));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ PointerEvents::Moved));
	}
}
