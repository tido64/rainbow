// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/File.h"

#include <gtest/gtest.h>

#include "Common/TypeCast.h"
#include "FileSystem/FileSystem.h"
#include "Tests/TestHelpers.h"

using rainbow::czstring;
using rainbow::File;
using rainbow::FileType;
using rainbow::SeekOrigin;
using rainbow::WriteableFile;
using rainbow::test::ScopedAssetsDirectory;

namespace
{
    constexpr size_t kTestFileSuccess = 0xabad1dea;

    class TestFileImpl
    {
    protected:
        static constexpr auto is_platform_handle() { return true; }

        static constexpr bool open(czstring, FileType, TestFileImpl&)
        {
            return true;
        }

        static auto resolve_path(czstring path, FileType) -> std::string
        {
            return path;
        }

        constexpr TestFileImpl() {}

        explicit TestFileImpl(File&&) noexcept {}

        auto handle() const -> PHYSFS_File* { return nullptr; }
        void set_handle(PHYSFS_File*) {}

        constexpr auto size() const -> size_t { return kTestFileSuccess; }

        void close() {}

        constexpr auto read(void*, size_t) const -> size_t
        {
            return kTestFileSuccess;
        }

        constexpr bool seek(int64_t, SeekOrigin) const { return true; }

        constexpr auto write(const void*, size_t) const -> size_t
        {
            return kTestFileSuccess;
        }

        constexpr explicit operator bool() const { return true; }
    };

    using TestFile = rainbow::TFile<TestFileImpl>;
    using TestWriteableFile = rainbow::TWriteableFile<TestFileImpl>;
}  // namespace

TEST(FileTest, HandlesInvalidPaths)
{
    ASSERT_FALSE(File::open("", FileType::Asset));
    ASSERT_FALSE(File::open("\0", FileType::Asset));
    ASSERT_FALSE(File::open("C:", FileType::Asset));
    ASSERT_FALSE(WriteableFile::open(""));
    ASSERT_FALSE(WriteableFile::open("\0"));
}

TEST(FileTest, HandlesEmptyFiles)
{
    ScopedAssetsDirectory scoped_assets{"FileTest_HandlesEmptyFiles"};
    std::string empty = "empty.dat";
    {
        ASSERT_FALSE(File::read(empty.c_str(), FileType::Asset));
    }
    {
        const auto& file = File::open(empty.c_str(), FileType::Asset);
        ASSERT_TRUE(file);
        ASSERT_EQ(file.size(), 0u);
    }
    {
        const auto& file = WriteableFile::open(empty.c_str());
        ASSERT_TRUE(file);
        ASSERT_EQ(file.size(), 0u);
    }
}

TEST(FileTest, SupportsPlatformImplementation)
{
    const auto& file = TestFile::open("/", FileType::Asset);

    ASSERT_TRUE(file);
    ASSERT_TRUE(file.seek(rainbow::kInvalidFileSize));
    ASSERT_EQ(file.size(), kTestFileSuccess);
    ASSERT_EQ(file.read(nullptr, rainbow::kInvalidFileSize), kTestFileSuccess);

    const auto& writeable_file = TestWriteableFile::open("/");

    ASSERT_TRUE(writeable_file);
    ASSERT_TRUE(writeable_file.seek(rainbow::kInvalidFileSize));
    ASSERT_EQ(writeable_file.size(), kTestFileSuccess);
    ASSERT_EQ(writeable_file.write(nullptr, rainbow::kInvalidFileSize),
              kTestFileSuccess);
}

TEST(FileTest, TranslatesSeekOrigin)
{
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::Current), SEEK_CUR);
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::End), SEEK_END);
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::Set), SEEK_SET);
}

TEST(FileTest, SeeksInFile)
{
    ScopedAssetsDirectory scoped_assets{"FileTest_SeeksInFile"};

    auto file = File::open("file", FileType::Asset);
    ASSERT_EQ(file.tell(), 0u);

    char pos = -1;
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '0');
    ASSERT_EQ(file.tell(), 1u);

    file.read(&pos, sizeof(pos));
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '2');
    ASSERT_EQ(file.tell(), 3u);

    file.seek(0, SeekOrigin::Current);
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '3');
    ASSERT_EQ(file.tell(), 4u);

    file.seek(1, SeekOrigin::Current);
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '5');
    ASSERT_EQ(file.tell(), 6u);

    file.seek(1, SeekOrigin::End);
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '9');
    ASSERT_EQ(file.tell(), 10u);

    file.seek(1, SeekOrigin::Set);
    file.read(&pos, sizeof(pos));
    ASSERT_EQ(pos, '1');
    ASSERT_EQ(file.tell(), 2u);

    for (auto i = 0u; i < 10; ++i)
    {
        file.seek(i);
        ASSERT_EQ(file.tell(), i);

        file.read(&pos, sizeof(pos));
        ASSERT_EQ(pos, '0' + rainbow::narrow_cast<char>(i));
    }

    for (auto i = 0u; i < 10; ++i)
    {
        file.seek(i, SeekOrigin::End);
        ASSERT_EQ(file.tell(), 10u - i);
    }
}
