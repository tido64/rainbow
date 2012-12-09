#include "Director.h"
#include "Input/Input.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"

#ifdef USE_HEIMDALL
#	define DISABLE_IF_USING_HEIMDALL(name) DISABLED_##name
#else
#	define DISABLE_IF_USING_HEIMDALL(name) name
#endif

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

	TouchTest() : flags(0)
	{
		Input::Instance->subscribe(this, Input::ALL_EVENTS);
	}

	~TouchTest()
	{
		Input::Instance->reset();
	}

	bool is_invalid(const Touch *const touches, const unsigned int count)
	{
		return count != 1 || touches[0].hash != 1 || touches[0].x != 2 || touches[0].y != 3;
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

private:
	unsigned int flags;
	Director director;
};

TEST_F(TouchTest, DISABLE_IF_USING_HEIMDALL(TouchBeganEvent))
{
	ASSERT_NE(nullptr, Input::Instance);
	Touch t1(2, 3);
	t1.hash = 1;
	Input::Instance->touch_began(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_BEGAN));
}

TEST_F(TouchTest, DISABLE_IF_USING_HEIMDALL(TouchCanceledEvent))
{
	ASSERT_NE(nullptr, Input::Instance);
	Input::Instance->touch_canceled();
	ASSERT_TRUE(is_triggered(TOUCH_CANCELED));
}

TEST_F(TouchTest, DISABLE_IF_USING_HEIMDALL(TouchEndedEvent))
{
	ASSERT_NE(nullptr, Input::Instance);
	Touch t1(2, 3);
	t1.hash = 1;
	Input::Instance->touch_ended(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_ENDED));
}

TEST_F(TouchTest, DISABLE_IF_USING_HEIMDALL(TouchMovedEvent))
{
	ASSERT_NE(nullptr, Input::Instance);
	Touch t1(2, 3);
	t1.hash = 1;
	Input::Instance->touch_moved(&t1, 1);
	ASSERT_TRUE(is_triggered(TOUCH_MOVED));
}

#undef DISABLE_IF_USING_HEIMDALL