// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Input/Input.h"
#include "Input/InputListener.h"
#include "Input/Pointer.h"

namespace
{
	class PointerTest : public InputListener, public ::testing::Test
	{
	public:
		struct Events
		{
			static const unsigned int Began     = 1u << 0;
			static const unsigned int Canceled  = 1u << 1;
			static const unsigned int Ended     = 1u << 2;
			static const unsigned int Moved     = 1u << 3;
		};

		PointerTest() : flags_(0) {}

		bool is_invalid(const ArrayView<Pointer>& pointers)
		{
			return pointers.size() != 1
			    || pointers[0].hash != 1
			    || pointers[0].x != 2
			    || pointers[0].y != 3
			    || pointers[0].timestamp != 0;
		}

		bool is_triggered(unsigned int flags)
		{
			return flags_ & flags;
		}

	protected:
		Input input;

		void SetUp() override { input.subscribe(this); }

	private:
		unsigned int flags_;

		bool on_pointer_began_impl(const ArrayView<Pointer>& pointers) override
		{
			if (is_invalid(pointers))
				return false;

			flags_ |= Events::Began;
			return true;
		}

		bool on_pointer_canceled_impl() override
		{
			flags_ |= Events::Canceled;
			return true;
		}

		bool on_pointer_ended_impl(const ArrayView<Pointer>& pointers) override
		{
			if (is_invalid(pointers))
				return false;

			flags_ |= Events::Ended;
			return true;
		}

		bool on_pointer_moved_impl(const ArrayView<Pointer>& pointers) override
		{
			if (is_invalid(pointers))
				return false;

			flags_ |= Events::Moved;
			return true;
		}
	};
}

TEST_F(PointerTest, IsZeroByDefault)
{
	const Pointer p;

	ASSERT_EQ(0u, p.hash);
	ASSERT_EQ(0, p.x);
	ASSERT_EQ(0, p.y);
	ASSERT_EQ(0ull, p.timestamp);
}

TEST_F(PointerTest, pointer_began)
{
	Pointer p1(1, 2, 3, 0);
	input.on_pointer_began({1, &p1});
	ASSERT_TRUE(is_triggered(Events::Began));
	ASSERT_FALSE(is_triggered(0xff ^ Events::Began));
}

TEST_F(PointerTest, pointer_canceled)
{
	input.on_pointer_canceled();
	ASSERT_TRUE(is_triggered(Events::Canceled));
	ASSERT_FALSE(is_triggered(0xff ^ Events::Canceled));
}

TEST_F(PointerTest, pointer_ended)
{
	Pointer p1(1, 2, 3, 0);
	input.on_pointer_ended({1, &p1});
	ASSERT_TRUE(is_triggered(Events::Ended));
	ASSERT_FALSE(is_triggered(0xff ^ Events::Ended));
}

TEST_F(PointerTest, pointer_moved)
{
	Pointer p1(1, 2, 3, 0);
	input.on_pointer_moved({1, &p1});
	ASSERT_TRUE(is_triggered(Events::Moved));
	ASSERT_FALSE(is_triggered(0xff ^ Events::Moved));
}
