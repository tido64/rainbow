#include <lua.hpp>

#include "Input/Input.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"
#include "Lua/lua_Input.h"

class TouchTest : public Touchable, public testing::Test
{
public:
	enum Events
	{
		TOUCH_BEGAN    = 1u << 0,
		TOUCH_CANCELED = 1u << 1,
		TOUCH_ENDED    = 1u << 2,
		TOUCH_MOVED    = 1u << 3
	};

	TouchTest() : flags(0), L(luaL_newstate()), input(L)
	{
		lua_createtable(this->L, 0, 1);
		Rainbow::Lua::Input::init(L);
		lua_setglobal(this->L, "rainbow");

		this->input.subscribe(this, Input::ALL_EVENTS);
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

	virtual void touch_began(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= TOUCH_BEGAN;
	}

	virtual void touch_canceled() override
	{
		this->flags |= TOUCH_CANCELED;
	}

	virtual void touch_ended(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= TOUCH_ENDED;
	}

	virtual void touch_moved(const Touch *const touches, const size_t count) override
	{
		if (is_invalid(touches, count))
			return;
		this->flags |= TOUCH_MOVED;
	}

protected:
	unsigned int flags;
	lua_State *L;
	Input input;
};

TEST_F(TouchTest, TouchBeganEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_began(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_BEGAN));
}

TEST_F(TouchTest, TouchCanceledEvent)
{
	this->input.touch_canceled();
	ASSERT_TRUE(is_triggered(TOUCH_CANCELED));
}

TEST_F(TouchTest, TouchEndedEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_ended(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_ENDED));
}

TEST_F(TouchTest, TouchMovedEvent)
{
	Touch t1(1, 2, 3, 0);
	this->input.touch_moved(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_MOVED));
}
