// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Input/Input.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

namespace
{
	class TouchTestFixture : public Touchable
	{
	public:
		struct Events
		{
			static const unsigned int TouchBegan     = 1u << 0;
			static const unsigned int TouchCanceled  = 1u << 1;
			static const unsigned int TouchEnded     = 1u << 2;
			static const unsigned int TouchMoved     = 1u << 3;
		};

		TouchTestFixture() : flags_(0) { }

		bool is_invalid(const Touch *const touches, const unsigned int count)
		{
			return count != 1
			    || touches[0].hash != 1
			    || touches[0].x != 2
			    || touches[0].y != 3
			    || touches[0].x0 != touches[0].x
			    || touches[0].y0 != touches[0].y
			    || touches[0].timestamp != 0;
		}

		bool is_triggered(const unsigned int flags)
		{
			return flags_ & flags;
		}

	private:
		unsigned int flags_;

		virtual void touch_began_impl(const Touch *const touches,
		                              const size_t count) override
		{
			if (is_invalid(touches, count))
				return;

			flags_ |= Events::TouchBegan;
		}

		virtual void touch_canceled_impl() override
		{
			flags_ |= Events::TouchCanceled;
		}

		virtual void touch_ended_impl(const Touch *const touches,
		                              const size_t count) override
		{
			if (is_invalid(touches, count))
				return;

			flags_ |= Events::TouchEnded;
		}

		virtual void touch_moved_impl(const Touch *const touches,
		                              const size_t count) override
		{
			if (is_invalid(touches, count))
				return;

			flags_ |= Events::TouchMoved;
		}
	};
}

TEST_CASE("Touch events", "[input]")
{
	using TouchEvents = TouchTestFixture::Events;

	lua_State *L = luaL_newstate();
	Input input(L);

	lua_createtable(L, 0, 1);
	Rainbow::Lua::Input::init(L);
	lua_setglobal(L, "rainbow");

	TouchTestFixture delegate;
	input.subscribe(&delegate, Input::Events::All);

	SECTION("touch_began event")
	{
		Touch t1(1, 2, 3, 0);
		input.touch_began(&t1, 1);
		REQUIRE(delegate.is_triggered(TouchEvents::TouchBegan));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ TouchEvents::TouchBegan));
	}

	SECTION("touch_canceled event")
	{
		input.touch_canceled();
		REQUIRE(delegate.is_triggered(TouchEvents::TouchCanceled));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ TouchEvents::TouchCanceled));
	}

	SECTION("touch_ended event")
	{
		Touch t1(1, 2, 3, 0);
		input.touch_ended(&t1, 1);
		REQUIRE(delegate.is_triggered(TouchEvents::TouchEnded));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ TouchEvents::TouchEnded));
	}

	SECTION("touch_moved event")
	{
		Touch t1(1, 2, 3, 0);
		input.touch_moved(&t1, 1);
		REQUIRE(delegate.is_triggered(TouchEvents::TouchMoved));
		REQUIRE_FALSE(delegate.is_triggered(0xff ^ TouchEvents::TouchMoved));
	}

	lua_close(L);
}
