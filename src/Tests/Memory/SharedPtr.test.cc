// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Memory/SharedPtr.h"

namespace
{
	class SharedPtrTestStruct : public RefCounted
	{
	public:
		SharedPtrTestStruct(bool &flag) : flag(flag) {}
		~SharedPtrTestStruct() { flag = true; }

	private:
		bool &flag;
	};
}

// TODO: The following newlines were added for Catch to create unique names.

TEST_CASE("SharedPtr is empty by default", "[sharedptr]")
{
	SharedPtr<SharedPtrTestStruct> empty;
	REQUIRE(empty.get() == nullptr);
}

TEST_CASE("SharedPtr manages reference counted objects", "[sharedptr]")
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr1(foo);
		REQUIRE(foo_ptr1.get() == foo);
		REQUIRE(foo_ptr1.use_count() == 1u);
		{
			SharedPtr<SharedPtrTestStruct> foo_ptr2(foo);
			REQUIRE(foo_ptr2.get() == foo);
			REQUIRE(foo_ptr1.use_count() == 2u);
			REQUIRE(foo_ptr2.use_count() == 2u);
			{
				SharedPtr<SharedPtrTestStruct> foo_ptr3(foo_ptr1);
				REQUIRE(foo_ptr3.get() == foo);
				REQUIRE(foo_ptr1.use_count() == 3u);
				REQUIRE(foo_ptr2.use_count() == 3u);
				REQUIRE(foo_ptr3.use_count() == 3u);
			}
			REQUIRE(foo_ptr1.use_count() == 2u);
			REQUIRE(foo_ptr2.use_count() == 2u);
		}
		REQUIRE(foo_ptr1.use_count() == 1u);
		REQUIRE_FALSE(foo_deleted);
	}
	REQUIRE(foo_deleted);
};

TEST_CASE("SharedPtr deletes managed object when reset", "[sharedptr]")
{
	bool foo_deleted = false;
	SharedPtr<SharedPtrTestStruct> foo_ptr(
	    new SharedPtrTestStruct(foo_deleted));
	foo_ptr.reset();
	REQUIRE(foo_ptr.get() == nullptr);
	REQUIRE(foo_deleted);
};

TEST_CASE("SharedPtr increments the counter on managed objects", "[sharedptr]")
{
	bool foo_deleted = false;
	bool bar_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	SharedPtrTestStruct *bar = new SharedPtrTestStruct(bar_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(bar_ptr.get() == bar);
		REQUIRE(foo_ptr.use_count() == 1u);
		REQUIRE(bar_ptr.use_count() == 1u);
		bar_ptr.reset(foo);
		REQUIRE(bar_ptr.get() == foo);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(bar_ptr.use_count() == 2u);
		REQUIRE(foo_ptr.use_count() == 2u);
		REQUIRE(bar_deleted);
	}
	REQUIRE(foo_deleted);
};

TEST_CASE("SharedPtr can be assigned another SharedPtr", "[sharedptr]")
{
	bool foo_deleted = false;
	bool bar_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	SharedPtrTestStruct *bar = new SharedPtrTestStruct(bar_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(bar_ptr.get() == bar);
		REQUIRE(foo_ptr.use_count() == 1u);
		REQUIRE(bar_ptr.use_count() == 1u);
		bar_ptr = foo_ptr;
		REQUIRE(bar_ptr.get() == foo);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(bar_ptr.use_count() == 2u);
		REQUIRE(foo_ptr.use_count() == 2u);
		REQUIRE(bar_deleted);
	}
	REQUIRE(foo_deleted);
};

TEST_CASE("SharedPtr can assign itself", "[sharedptr]")
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(foo_ptr.use_count() == 1u);
		foo_ptr.reset(foo);
		REQUIRE_FALSE(foo_deleted);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(foo_ptr.use_count() == 1u);
		foo_ptr = foo_ptr;
		REQUIRE_FALSE(foo_deleted);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(foo_ptr.use_count() == 1u);
	}
	REQUIRE(foo_deleted);
};

TEST_CASE("SharedPtr can move other SharedPtrs", "[sharedptr]")
{
	bool foo_deleted = false;
	SharedPtrTestStruct *foo = new SharedPtrTestStruct(foo_deleted);
	{
		SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(foo_ptr.use_count() == 1u);

		SharedPtr<SharedPtrTestStruct> bar_ptr(std::move(foo_ptr));
		REQUIRE_FALSE(foo_deleted);
		REQUIRE(foo_ptr.get() == nullptr);
		REQUIRE(bar_ptr.get() == foo);
		REQUIRE(bar_ptr.use_count() == 1u);

		foo_ptr = std::move(bar_ptr);
		REQUIRE_FALSE(foo_deleted);
		REQUIRE(bar_ptr.get() == nullptr);
		REQUIRE(foo_ptr.get() == foo);
		REQUIRE(foo_ptr.use_count() == 1u);
	}
	REQUIRE(foo_deleted);
};
