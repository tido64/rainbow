// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Input/Input.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

class TouchTest : public Touchable, public testing::Test
{
public:
	struct Events
	{
		static const unsigned int TouchBegan     = 1u << 0;
		static const unsigned int TouchCanceled  = 1u << 1;
		static const unsigned int TouchEnded     = 1u << 2;
		static const unsigned int TouchMoved     = 1u << 3;
	};

	TouchTest() : flags(0), L(luaL_newstate()), input(L)
	{
		lua_createtable(this->L, 0, 1);
		Rainbow::Lua::Input::init(L);
		lua_setglobal(this->L, "rainbow");

		this->input.subscribe(this, Input::Events::All);
	}

	~TouchTest()
	{
		lua_close(this->L);
	}

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
		return this->flags & flags;
	}

protected:
	unsigned int flags;
	lua_State *L;
	Input input;

private:
	virtual void touch_began_impl(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= Events::TouchBegan;
	}

	virtual void touch_canceled_impl() override
	{
		this->flags |= Events::TouchCanceled;
	}

	virtual void touch_ended_impl(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= Events::TouchEnded;
	}

	virtual void touch_moved_impl(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= Events::TouchMoved;
	}
};

TEST_F(TouchTest, TouchBeganEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_began(&t1, 1);
	ASSERT_TRUE(is_triggered(Events::TouchBegan));
}

TEST_F(TouchTest, TouchCanceledEvent)
{
	this->input.touch_canceled();
	ASSERT_TRUE(is_triggered(Events::TouchCanceled));
}

TEST_F(TouchTest, TouchEndedEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_ended(&t1, 1);
	ASSERT_TRUE(is_triggered(Events::TouchEnded));
}

TEST_F(TouchTest, TouchMovedEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_moved(&t1, 1);
	ASSERT_TRUE(is_triggered(Events::TouchMoved));
}
