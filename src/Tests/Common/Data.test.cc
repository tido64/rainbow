// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"

#include <cstring>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"

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

    struct DataTestTypeNames
    {
        template <typename T>
        static auto GetName(int) -> std::string
        {
            if constexpr (std::is_same_v<T, FromBytes>)
                return "RawBytes";
            else if constexpr (std::is_same_v<T, FromLiteral>)
                return "StringLiteral";
        }
    };
}  // namespace

TYPED_TEST_CASE(DataTest, DataTestTypes, DataTestTypeNames);

TYPED_TEST(DataTest, EncapsulatesInMemoryData)
{
    Data blob = this->data_();

    ASSERT_TRUE(blob);
    ASSERT_EQ(blob.as<char*>(), kSecretData);
    ASSERT_EQ(blob.size(), this->data_.size());
    ASSERT_EQ(memcmp(blob.bytes(), kSecretData, blob.size()), 0);

    Data moved_blob{std::move(blob)};

    ASSERT_FALSE(blob);
    ASSERT_EQ(blob.bytes(), nullptr);
    ASSERT_EQ(blob.size(), 0u);
    ASSERT_EQ(moved_blob.as<char*>(), kSecretData);
    ASSERT_EQ(moved_blob.size(), this->data_.size());
    ASSERT_EQ(memcmp(moved_blob.bytes(), kSecretData, moved_blob.size()), 0);
}
