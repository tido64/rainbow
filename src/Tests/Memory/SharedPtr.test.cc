// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Memory/SharedPtr.h"

#include <gtest/gtest.h>

using rainbow::RefCounted;
using rainbow::SharedPtr;

namespace
{
    class SharedPtrTestStruct : public RefCounted
    {
    public:
        explicit SharedPtrTestStruct(bool& flag) : flag(flag) {}
        ~SharedPtrTestStruct() { flag = true; }

    private:
        bool& flag;
    };
}  // namespace

TEST(SharedPtrTest, IsEmptyByDefault)
{
    SharedPtr<SharedPtrTestStruct> empty;
    ASSERT_FALSE(empty);
    ASSERT_EQ(empty.get(), nullptr);
}

TEST(SharedPtrTest, ManagesReferenceCountedObjects)
{
    bool foo_deleted = false;
    auto foo = new SharedPtrTestStruct(foo_deleted);
    {
        SharedPtr<SharedPtrTestStruct> foo_ptr1(foo);
        ASSERT_EQ(foo_ptr1.get(), foo);
        ASSERT_EQ(foo_ptr1.use_count(), 1u);
        {
            SharedPtr<SharedPtrTestStruct> foo_ptr2(foo);
            ASSERT_EQ(foo_ptr2.get(), foo);
            ASSERT_EQ(foo_ptr1.use_count(), 2u);
            ASSERT_EQ(foo_ptr2.use_count(), 2u);
            {
                SharedPtr<SharedPtrTestStruct> foo_ptr3(foo_ptr1);
                ASSERT_EQ(foo_ptr3.get(), foo);
                ASSERT_EQ(foo_ptr1.use_count(), 3u);
                ASSERT_EQ(foo_ptr2.use_count(), 3u);
                ASSERT_EQ(foo_ptr3.use_count(), 3u);
            }
            ASSERT_EQ(foo_ptr1.use_count(), 2u);
            ASSERT_EQ(foo_ptr2.use_count(), 2u);
        }
        ASSERT_EQ(foo_ptr1.use_count(), 1u);
        ASSERT_FALSE(foo_deleted);
    }
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, DeletesManagedObjectWhenReset)
{
    bool foo_deleted = false;
    SharedPtr<SharedPtrTestStruct> foo_ptr(
        new SharedPtrTestStruct(foo_deleted));
    ASSERT_TRUE(foo_ptr);
    foo_ptr.reset();
    ASSERT_FALSE(foo_ptr);
    ASSERT_EQ(foo_ptr.get(), nullptr);
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, ResetsWithNull)
{
    bool foo_deleted = false;
    SharedPtr<SharedPtrTestStruct> foo_ptr(
        new SharedPtrTestStruct(foo_deleted));
    ASSERT_TRUE(foo_ptr);
    foo_ptr.reset(nullptr);
    ASSERT_FALSE(foo_ptr);
    ASSERT_EQ(foo_ptr.get(), nullptr);
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, IncrementsTheCounterOnManagedObjects)
{
    bool foo_deleted = false;
    bool bar_deleted = false;
    auto foo = new SharedPtrTestStruct(foo_deleted);
    auto bar = new SharedPtrTestStruct(bar_deleted);
    {
        SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
        SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(bar_ptr.get(), bar);
        ASSERT_EQ(foo_ptr.use_count(), 1u);
        ASSERT_EQ(bar_ptr.use_count(), 1u);
        bar_ptr.reset(foo);
        ASSERT_EQ(bar_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(bar_ptr.use_count(), 2u);
        ASSERT_EQ(foo_ptr.use_count(), 2u);
        ASSERT_TRUE(bar_deleted);
    }
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, Assigns)
{
    bool foo_deleted = false;
    bool bar_deleted = false;
    auto foo = new SharedPtrTestStruct(foo_deleted);
    auto bar = new SharedPtrTestStruct(bar_deleted);
    {
        SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
        SharedPtr<SharedPtrTestStruct> bar_ptr(bar);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(bar_ptr.get(), bar);
        ASSERT_EQ(foo_ptr.use_count(), 1u);
        ASSERT_EQ(bar_ptr.use_count(), 1u);
        bar_ptr = foo_ptr;
        ASSERT_EQ(bar_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(bar_ptr.use_count(), 2u);
        ASSERT_EQ(foo_ptr.use_count(), 2u);
        ASSERT_TRUE(bar_deleted);
    }
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, AssignsItself)
{
    bool foo_deleted = false;
    auto foo = new SharedPtrTestStruct(foo_deleted);
    {
        SharedPtr<SharedPtrTestStruct> foo_ptr(foo);

        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.use_count(), 1u);

        foo_ptr.reset(foo);

        ASSERT_FALSE(foo_deleted);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.use_count(), 1u);

#if defined(__clang__) && __apple_build_version__ >= 10000000
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wself-assign-overloaded"
#endif
        foo_ptr = foo_ptr;
#if defined(__clang__) && __apple_build_version__ >= 10000000
#    pragma clang diagnostic pop
#endif

        ASSERT_FALSE(foo_deleted);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.use_count(), 1u);
    }
    ASSERT_TRUE(foo_deleted);
}

TEST(SharedPtrTest, MoveSemantics)
{
    bool foo_deleted = false;
    auto foo = new SharedPtrTestStruct(foo_deleted);
    {
        SharedPtr<SharedPtrTestStruct> foo_ptr(foo);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.use_count(), 1u);

        SharedPtr<SharedPtrTestStruct> bar_ptr(std::move(foo_ptr));
        ASSERT_FALSE(foo_deleted);
        ASSERT_EQ(foo_ptr.get(), nullptr);
        ASSERT_EQ(bar_ptr.get(), foo);
        ASSERT_EQ(bar_ptr.use_count(), 1u);

        foo_ptr = std::move(bar_ptr);
        ASSERT_FALSE(foo_deleted);
        ASSERT_EQ(bar_ptr.get(), nullptr);
        ASSERT_EQ(foo_ptr.get(), foo);
        ASSERT_EQ(foo_ptr.use_count(), 1u);
    }
    ASSERT_TRUE(foo_deleted);
}
