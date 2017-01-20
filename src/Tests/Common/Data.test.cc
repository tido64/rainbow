// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "FileSystem/FileSystem.h"

using rainbow::Data;

namespace
{
    const char kSecretData[] = "It's a double-rainbow!\n";

    class DataReadWriteTest : public ::testing::Test
    {
    public:
        DataReadWriteTest()
            : blob_(kSecretData,
                    rainbow::array_size(kSecretData),
                    Data::Ownership::Reference) {}

    protected:
        auto blob() const -> const Data& { return blob_; }

    private:
        Data blob_;
    };
}

TEST(DataTest, ReferencesInMemoryData)
{
    Data blob = Data::from_literal(kSecretData);
    ASSERT_EQ(kSecretData, blob.bytes());
    ASSERT_EQ(rainbow::array_size(kSecretData) - 1, blob.size());
    ASSERT_EQ(0, memcmp(blob, kSecretData, blob.size()));
}

TEST_F(DataReadWriteTest, ManagesArbitraryDataBlobs)
{
    ASSERT_EQ(rainbow::array_size(kSecretData), blob().size());
    ASSERT_EQ(0, strcmp(blob(), kSecretData));
}

TEST_F(DataReadWriteTest, WritesAndReadsFromDisk)
{
    const char file[] = "Rainbow__Data.test";

    ASSERT_TRUE(blob().save(file));

    const Data& from_disk = Data::load_document(file);

    ASSERT_TRUE(from_disk);
    ASSERT_EQ(0, strcmp(from_disk, kSecretData));

    std::error_code error;
    rainbow::filesystem::remove(rainbow::filesystem::user(file), error);
}
