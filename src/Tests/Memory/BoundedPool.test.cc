// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <array>

#include <gtest/gtest.h>

#include "Memory/BoundedPool.h"

using rainbow::BoundedPool;

TEST(BoundedPoolTest, EmptyByDefault)
{
    ASSERT_TRUE(BoundedPool<int>{}.empty());
    ASSERT_EQ(BoundedPool<int>{}.next(), nullptr);
}

TEST(BoundedPoolTest, GeneratesNItems)
{
    constexpr int kCount = 10;

    BoundedPool<int> pool{[i = 0]() mutable { return i++; }, kCount};
    for (int i = 0; i < kCount; ++i)
    {
        ASSERT_FALSE(pool.empty());
        auto item = pool.next();
        ASSERT_NE(item, nullptr);
        ASSERT_EQ(*item, i);
    }

    ASSERT_TRUE(pool.empty());
    ASSERT_EQ(pool.next(), nullptr);
}

TEST(BoundedPoolTest, IteratesOverAllItems)
{
    constexpr int kCount = 10;

    BoundedPool<int> pool{[i = 0]() mutable { return i++; }, kCount};
    int j = -1;
    for_each(pool, [&j](auto&& i) mutable { ASSERT_EQ(i, ++j); });
}

TEST(BoundedPoolTest, ReturnsItemsToThePool)
{
    constexpr int kCount = 10;

    std::array<int*, kCount> store;
    BoundedPool<int> pool{[i = 0]() mutable { return i++; }, kCount};
    for (int i = 0; i < kCount; ++i)
        store[i] = pool.next();

    ASSERT_TRUE(pool.empty());
    ASSERT_EQ(pool.next(), nullptr);

    for (auto&& i : store)
        pool.release(*i);

    ASSERT_FALSE(pool.empty());

    for (int i = 0; i < kCount; ++i)
    {
        store[i] = pool.next();
        ASSERT_EQ(*store[i], kCount - i - 1);
    }

    ASSERT_TRUE(pool.empty());
    ASSERT_EQ(pool.next(), nullptr);

    for (auto&& i : store)
    {
        pool.release(*i);
        ASSERT_FALSE(pool.empty());
    }

    for (int i = 0; i < kCount; ++i)
        ASSERT_EQ(*pool.next(), i);

    ASSERT_TRUE(pool.empty());
    ASSERT_EQ(pool.next(), nullptr);
}
