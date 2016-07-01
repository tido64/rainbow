// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Random.h"
#include "Memory/StableArray.h"

namespace
{
    template <size_t N>
    struct SizableStruct
    {
        size_t id;
        char padding[N];
    };

    template <size_t Padding, uint32_t Size>
    void test_alignment()
    {
        StableArray<SizableStruct<Padding>> array(Size);

        ASSERT_EQ(Size, array.size());

        const auto header_size = reinterpret_cast<uintptr_t>(array.data()) -
                                 reinterpret_cast<uintptr_t>(&array);

        ASSERT_LE(Size * sizeof(array[0]), header_size);
        ASSERT_EQ(0u, header_size % alignof(SizableStruct<Padding>));
        ASSERT_EQ(0u,
                  reinterpret_cast<uintptr_t>(array.data()) %
                      alignof(SizableStruct<Padding>));
    }
}

TEST(StableArrayTest, AllocatesAlignedStore)
{
    test_alignment<23, 11>();
    test_alignment<7, 18>();
}

TEST(StableArrayTest, FindsIterators)
{
    StableArray<SizableStruct<5>> array(6);
    for_each(array, [i = 9](auto&& s) mutable { s.id = ++i; });

    for (uint32_t i = 0; i < array.size(); ++i)
    {
        auto iter = array.find_iterator(i);
        ASSERT_EQ(i + 10, array[iter].id);
    }
}

TEST(StableArrayTest, IteratesWithForEach)
{
    StableArray<SizableStruct<5>> array(6);
    for_each(array, [i = 0](auto&& s) mutable { s.id = i++; });

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);
}

TEST(StableArrayTest, IteratorsAreStableAfterMoves)
{
    StableArray<SizableStruct<5>> array(6);
    for_each(array, [i = 0](auto&& s) mutable { s.id = i++; });

    array.move(1, 4);  // -> 0 2 3 4 1 5

    ASSERT_EQ(0u, array.data()[0].id);
    ASSERT_EQ(2u, array.data()[1].id);
    ASSERT_EQ(3u, array.data()[2].id);
    ASSERT_EQ(4u, array.data()[3].id);
    ASSERT_EQ(1u, array.data()[4].id);
    ASSERT_EQ(5u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.move(2, 4);  // -> 0 3 4 1 2 5

    ASSERT_EQ(0u, array.data()[0].id);
    ASSERT_EQ(3u, array.data()[1].id);
    ASSERT_EQ(4u, array.data()[2].id);
    ASSERT_EQ(1u, array.data()[3].id);
    ASSERT_EQ(2u, array.data()[4].id);
    ASSERT_EQ(5u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.move(5, 5);  // -> 0 3 4 1 2 5

    ASSERT_EQ(0u, array.data()[0].id);
    ASSERT_EQ(3u, array.data()[1].id);
    ASSERT_EQ(4u, array.data()[2].id);
    ASSERT_EQ(1u, array.data()[3].id);
    ASSERT_EQ(2u, array.data()[4].id);
    ASSERT_EQ(5u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.move(5, 0);  // -> 5 0 3 4 1 2

    ASSERT_EQ(5u, array.data()[0].id);
    ASSERT_EQ(0u, array.data()[1].id);
    ASSERT_EQ(3u, array.data()[2].id);
    ASSERT_EQ(4u, array.data()[3].id);
    ASSERT_EQ(1u, array.data()[4].id);
    ASSERT_EQ(2u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.move(3, 1);  // -> 5 3 0 4 1 2

    ASSERT_EQ(5u, array.data()[0].id);
    ASSERT_EQ(3u, array.data()[1].id);
    ASSERT_EQ(0u, array.data()[2].id);
    ASSERT_EQ(4u, array.data()[3].id);
    ASSERT_EQ(1u, array.data()[4].id);
    ASSERT_EQ(2u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.move(3, 2);  // -> 5 0 3 4 1 2

    ASSERT_EQ(5u, array.data()[0].id);
    ASSERT_EQ(0u, array.data()[1].id);
    ASSERT_EQ(3u, array.data()[2].id);
    ASSERT_EQ(4u, array.data()[3].id);
    ASSERT_EQ(1u, array.data()[4].id);
    ASSERT_EQ(2u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);
}

TEST(StableArrayTest, IteratorsAreStableAfterSwaps)
{
    StableArray<SizableStruct<5>> array(6);
    for_each(array, [i = 0](auto&& s) mutable { s.id = i++; });

    array.swap(1, 5);

    ASSERT_EQ(0u, array.data()[0].id);
    ASSERT_EQ(5u, array.data()[1].id);
    ASSERT_EQ(2u, array.data()[2].id);
    ASSERT_EQ(3u, array.data()[3].id);
    ASSERT_EQ(4u, array.data()[4].id);
    ASSERT_EQ(1u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.swap(1, 5);

    for (uint32_t i = 0; i < array.size(); ++i)
    {
        ASSERT_EQ(i, array.data()[i].id);
        ASSERT_EQ(i, array[i].id);
    }

    array.swap(3, 0);
    array.swap(2, 5);
    array.swap(2, 4);

    ASSERT_EQ(3u, array.data()[0].id);
    ASSERT_EQ(1u, array.data()[1].id);
    ASSERT_EQ(5u, array.data()[2].id);
    ASSERT_EQ(0u, array.data()[3].id);
    ASSERT_EQ(2u, array.data()[4].id);
    ASSERT_EQ(4u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    array.swap(3, 3);

    ASSERT_EQ(3u, array.data()[0].id);
    ASSERT_EQ(1u, array.data()[1].id);
    ASSERT_EQ(5u, array.data()[2].id);
    ASSERT_EQ(0u, array.data()[3].id);
    ASSERT_EQ(2u, array.data()[4].id);
    ASSERT_EQ(4u, array.data()[5].id);

    for (uint32_t i = 0; i < array.size(); ++i)
        ASSERT_EQ(i, array[i].id);

    rainbow::Random random;
    random.seed();
    for (uint32_t p = 0; p < 720; ++p)
    {
        array.swap(random(array.size()), random(array.size()));
        for (uint32_t i = 0; i < array.size(); ++i)
            ASSERT_EQ(i, array[i].id);
    }
}
