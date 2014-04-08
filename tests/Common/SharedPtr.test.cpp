// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/SharedPtr.h"

namespace
{
	class SharedPtrTestStruct : public RefCounted
	{
	public:
		SharedPtrTestStruct(bool &flag) : flag(flag) { }
		~SharedPtrTestStruct() { flag = true; }

	private:
		bool &flag;
	};
}

TEST(SharedPtrTest, Default)
{
	SharedPtr<SharedPtrTestStruct> empty;
	ASSERT_EQ(nullptr, empty.get());
}

TEST(SharedPtrTest, Counting)
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr1(foo);
		ASSERT_EQ(foo, foo_ptr1.get());
		ASSERT_EQ(1u, foo_ptr1.ref_count());
		{
			SharedPtr<SharedPtrTestStruct> foo_ptr2(foo);
			ASSERT_EQ(foo, foo_ptr2.get());
			ASSERT_EQ(2u, foo_ptr1.ref_count());
			ASSERT_EQ(2u, foo_ptr2.ref_count());
			{
				SharedPtr<SharedPtrTestStruct> foo_ptr3(foo_ptr1);
				ASSERT_EQ(foo, foo_ptr3.get());
				ASSERT_EQ(3u, foo_ptr1.ref_count());
				ASSERT_EQ(3u, foo_ptr2.ref_count());
				ASSERT_EQ(3u, foo_ptr3.ref_count());
			}
			ASSERT_EQ(2u, foo_ptr1.ref_count());
			ASSERT_EQ(2u, foo_ptr2.ref_count());
		}
		ASSERT_EQ(1u, foo_ptr1.ref_count());
		ASSERT_FALSE(foo_deleted);
	}
	ASSERT_TRUE(foo_deleted);
};

TEST(SharedPtrTest, AssignNullPtr)
{
	bool foo_deleted = false;
	SharedPtr<SharedPtrTestStruct> foo_ptr(
	    new SharedPtrTestStruct(foo_deleted));
	foo_ptr = nullptr;
	ASSERT_EQ(nullptr, foo_ptr.get());
	ASSERT_TRUE(foo_deleted);
};

TEST(SharedPtrTest, AssignRawPtr)
{
	bool foo_deleted = false;
	bool bar_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	SharedPtrTestStruct *bar = new SharedPtrTestStruct(bar_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(bar, bar_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
		ASSERT_EQ(1u, bar_ptr.ref_count());
		bar_ptr = foo;
		ASSERT_EQ(foo, bar_ptr.get());
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(2u, bar_ptr.ref_count());
		ASSERT_EQ(2u, foo_ptr.ref_count());
		ASSERT_TRUE(bar_deleted);
	}
	ASSERT_TRUE(foo_deleted);
};

TEST(SharedPtrTest, AssignSharedPtr)
{
	bool foo_deleted = false;
	bool bar_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	SharedPtrTestStruct *bar = new SharedPtrTestStruct(bar_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(bar, bar_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
		ASSERT_EQ(1u, bar_ptr.ref_count());
		bar_ptr = foo_ptr;
		ASSERT_EQ(foo, bar_ptr.get());
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(2u, bar_ptr.ref_count());
		ASSERT_EQ(2u, foo_ptr.ref_count());
		ASSERT_TRUE(bar_deleted);
	}
	ASSERT_TRUE(foo_deleted);
};

TEST(SharedPtrTest, AssignSelf)
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
		foo_ptr = foo;
		ASSERT_FALSE(foo_deleted);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
		foo_ptr = foo_ptr;
		ASSERT_FALSE(foo_deleted);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
	}
	ASSERT_TRUE(foo_deleted);
};

TEST(SharedPtrTest, MoveSemantics)
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());

		SharedPtr<SharedPtrTestStruct> bar_ptr(std::move(foo_ptr));
		ASSERT_FALSE(foo_deleted);
		ASSERT_EQ(nullptr, foo_ptr.get());
		ASSERT_EQ(foo, bar_ptr.get());
		ASSERT_EQ(1u, bar_ptr.ref_count());

		foo_ptr = std::move(bar_ptr);
		ASSERT_FALSE(foo_deleted);
		ASSERT_EQ(nullptr, bar_ptr.get());
		ASSERT_EQ(foo, foo_ptr.get());
		ASSERT_EQ(1u, foo_ptr.ref_count());
	}
	ASSERT_TRUE(foo_deleted);
};
