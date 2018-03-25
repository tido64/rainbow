// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"

using rainbow::Data;

namespace
{
    constexpr char kSecretData[] = "It's a double-rainbow!\n";

    struct FromBytes
    {
        auto size() { return rainbow::array_size(kSecretData); }
        auto operator()() -> Data { return Data::from_bytes(kSecretData); }
    };

    struct FromLiteral
    {
        auto size() { return rainbow::array_size(kSecretData) - 1; }
        auto operator()() -> Data { return Data::from_literal(kSecretData); }
    };

    template <typename T>
    class DataTest : public ::testing::Test
    {
    protected:
        T data_;
    };

    using DataTestTypes = ::testing::Types<FromBytes, FromLiteral>;
}  // namespace

TYPED_TEST_CASE(DataTest, DataTestTypes);

TYPED_TEST(DataTest, EncapsulatesInMemoryData)
{
    Data blob = this->data_();

    ASSERT_TRUE(blob);
    ASSERT_EQ(kSecretData, blob.as<char*>());
    ASSERT_EQ(this->data_.size(), blob.size());
    ASSERT_EQ(0, memcmp(blob.bytes(), kSecretData, blob.size()));

    Data moved_blob{std::move(blob)};

    ASSERT_FALSE(blob);
    ASSERT_EQ(nullptr, blob.bytes());
    ASSERT_EQ(0u, blob.size());
    ASSERT_EQ(kSecretData, moved_blob.as<char*>());
    ASSERT_EQ(this->data_.size(), moved_blob.size());
    ASSERT_EQ(0, memcmp(moved_blob.bytes(), kSecretData, moved_blob.size()));
}
