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
}

TEST(DataTest, ReferencesInMemoryData)
{
    const Data& blob = Data::from_literal(kSecretData);
    ASSERT_EQ(kSecretData, blob.as<char*>());
    ASSERT_EQ(rainbow::array_size(kSecretData) - 1, blob.size());
    ASSERT_EQ(0, memcmp(blob.bytes(), kSecretData, blob.size()));
}

TEST(DataTest, ManagesArbitraryDataBlobs)
{
    const Data& blob = Data::from_bytes(kSecretData);
    ASSERT_EQ(kSecretData, blob.as<char*>());
    ASSERT_EQ(rainbow::array_size(kSecretData), blob.size());
    ASSERT_EQ(0, memcmp(blob.bytes(), kSecretData, blob.size()));
}
