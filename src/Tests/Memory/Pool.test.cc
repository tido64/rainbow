// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Memory/Pool.h"

namespace
{
    constexpr int kDisposed = -1;

    template <typename T>
    class PoolTest : public ::testing::Test
    {
    };

    class Integer
    {
    public:
        Integer() : value_(0) { NOT_USED(padding_); }
        explicit Integer(int value) : value_(value) {}

        auto is_disposed() const { return value_ == kDisposed; }

        void dispose() { value_ = kDisposed; }
        void reset(int new_value) { value_ = new_value; }

        operator int() const { return value_; }

    private:
        int value_;
        int padding_[7];
    };

    using Types = ::testing::Types<Integer>;

    struct PoolTestTypeNames
    {
        template <typename T>
        static auto GetName(int) -> std::string
        {
            if constexpr (std::is_same_v<T, Integer>)
                return "Integer";
        }
    };
}  // namespace

TYPED_TEST_CASE(PoolTest, Types, PoolTestTypeNames);

TYPED_TEST(PoolTest, ClearsElements)
{
    rainbow::Pool<TypeParam> pool;
    auto item0 = pool.construct(0);
    auto item1 = pool.construct(1);

    ASSERT_EQ(pool.size(), 2u);
    ASSERT_EQ(*item0, TypeParam{0});
    ASSERT_EQ(*item1, TypeParam{1});

    pool.clear();

    ASSERT_EQ(pool.size(), 0u);
}

TYPED_TEST(PoolTest, ReusesElements)
{
    rainbow::Pool<TypeParam> pool;
    TypeParam* arr[5]{};

    for (size_t i = 0; i < rainbow::array_size(arr); ++i)
    {
        ASSERT_EQ(pool.size(), i);
        arr[i] = pool.construct(static_cast<int>(i));
    }

    pool.release(arr[2]);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{0});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{kDisposed});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{4});

    pool.release(arr[0]);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{kDisposed});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{kDisposed});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{4});

    pool.release(arr[4]);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{kDisposed});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{kDisposed});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{kDisposed});

    arr[0] = pool.construct(4);
    arr[2] = pool.construct(6);
    arr[4] = pool.construct(8);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{4});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{6});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{8});
}

TYPED_TEST(PoolTest, IteratesOnlyActiveElements)
{
    rainbow::Pool<TypeParam> pool;
    TypeParam* arr[]{pool.construct(0),
                     pool.construct(1),
                     pool.construct(2),
                     pool.construct(3),
                     pool.construct(4)};

    pool.release(arr[2]);
    pool.release(arr[4]);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{0});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{kDisposed});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{kDisposed});

    size_t i = 0;
    for_each(pool, [&i](TypeParam& item) {
        ASSERT_FALSE(item.is_disposed());
        ++i;
    });

    ASSERT_EQ(i, pool.size() - 2);

    arr[4] = pool.construct(6);

    ASSERT_EQ(pool.size(), rainbow::array_size(arr));
    ASSERT_EQ(*arr[0], TypeParam{0});
    ASSERT_EQ(*arr[1], TypeParam{1});
    ASSERT_EQ(*arr[2], TypeParam{kDisposed});
    ASSERT_EQ(*arr[3], TypeParam{3});
    ASSERT_EQ(*arr[4], TypeParam{6});

    i = 0;
    for_each(pool, [&i](TypeParam& item) {
        ASSERT_FALSE(item.is_disposed());
        ++i;
    });

    ASSERT_EQ(i, pool.size() - 1);
}
