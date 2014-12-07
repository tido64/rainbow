// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/TimedEvent.h"

namespace
{
	class TimedEventTest : public TimedEvent<TimedEventTest>
	{
	public:
		TimedEventTest() : TimedEvent(1), ticks_(0) {}

		void tick()
		{
			++ticks_;
		}

		int ticks() const
		{
			return ticks_;
		}

		void on_start() {}
		void on_stop() {}

	private:
		int ticks_;
	};
}

// TODO: The following newlines were added for Catch to create unique names.


TEST_CASE("TimedEvents are started on creation", "[timedevent]")
{
	TimedEventTest event;
	REQUIRE_FALSE(event.is_stopped());
}

TEST_CASE("TimedEvent calls tick() on timeout", "[timedevent]")
{
	TimedEventTest event;
	for (int i = 1; i <= 5; ++i)
	{
		event.update(1);
		REQUIRE(event.ticks() == i);
	}
}

// TODO: The following newlines were added for Catch to create unique names.


TEST_CASE("TimedEvent calls tick() multiple times to catch up", "[timedevent]")
{
	const int dt = 8;
	TimedEventTest event;
	event.update(dt);
	REQUIRE(event.ticks() == dt);
}

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("TimedEvents can be stopped and restarted", "[timedevent]")
{
	const int dt = 8;
	TimedEventTest event;
	event.update(dt);
	const int ticks = event.ticks();
	event.stop();
	REQUIRE(event.is_stopped());
	event.update(dt);
	REQUIRE(event.ticks() == ticks);
	event.start();
	event.update(dt);
	REQUIRE(event.ticks() == dt * 2);
}
