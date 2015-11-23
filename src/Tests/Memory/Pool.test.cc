// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Memory/Pool.h"

namespace
{
	constexpr int kDisposed = -1;

	template <typename T>
	class PoolTest : public ::testing::Test {};
}

class Integer
{
public:
	Integer() : value_(0) {}
	Integer(int value) : value_(value) {}
	Integer(size_t value) : value_(static_cast<int>(value)) {}

	void dispose() { value_ = kDisposed; }

	operator int() const { return value_; }

private:
	int value_;
	int padding_[7];
};

using Types = ::testing::Types<Integer>;
TYPED_TEST_CASE(PoolTest, Types);

TYPED_TEST(PoolTest, ClearsElements)
{
	rainbow::Pool<TypeParam> pool;
	auto item0 = pool.construct(0);
	auto item1 = pool.construct(1);

	ASSERT_EQ(2u, pool.size());
	ASSERT_EQ(TypeParam(0), *item0);
	ASSERT_EQ(TypeParam(1), *item1);

	pool.clear();

	ASSERT_EQ(0u, pool.size());
}

TYPED_TEST(PoolTest, ReusesElements)
{
	rainbow::Pool<TypeParam> pool;
	TypeParam* arr[5]{};

	for (size_t i = 0; i < rainbow::array_size(arr); ++i)
	{
		ASSERT_EQ(i, pool.size());
		arr[i] = pool.construct(i);
	}

	pool.release(arr[2]);

	ASSERT_EQ(rainbow::array_size(arr), pool.size());
	ASSERT_EQ(TypeParam(0), *arr[0]);
	ASSERT_EQ(TypeParam(1), *arr[1]);
	ASSERT_EQ(TypeParam(kDisposed), *arr[2]);
	ASSERT_EQ(TypeParam(3), *arr[3]);
	ASSERT_EQ(TypeParam(4), *arr[4]);

	pool.release(arr[0]);

	ASSERT_EQ(rainbow::array_size(arr), pool.size());
	ASSERT_EQ(TypeParam(kDisposed), *arr[0]);
	ASSERT_EQ(TypeParam(1), *arr[1]);
	ASSERT_EQ(TypeParam(kDisposed), *arr[2]);
	ASSERT_EQ(TypeParam(3), *arr[3]);
	ASSERT_EQ(TypeParam(4), *arr[4]);

	pool.release(arr[4]);

	ASSERT_EQ(rainbow::array_size(arr), pool.size());
	ASSERT_EQ(TypeParam(kDisposed), *arr[0]);
	ASSERT_EQ(TypeParam(1), *arr[1]);
	ASSERT_EQ(TypeParam(kDisposed), *arr[2]);
	ASSERT_EQ(TypeParam(3), *arr[3]);
	ASSERT_EQ(TypeParam(kDisposed), *arr[4]);

	arr[0] = pool.construct(4);
	arr[2] = pool.construct(6);
	arr[4] = pool.construct(8);

	ASSERT_EQ(rainbow::array_size(arr), pool.size());
	ASSERT_EQ(TypeParam(4), *arr[0]);
	ASSERT_EQ(TypeParam(1), *arr[1]);
	ASSERT_EQ(TypeParam(6), *arr[2]);
	ASSERT_EQ(TypeParam(3), *arr[3]);
	ASSERT_EQ(TypeParam(8), *arr[4]);
}
