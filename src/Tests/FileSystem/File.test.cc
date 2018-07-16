// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Random.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

#ifdef RAINBOW_OS_UNIX
#   define _rmdir rmdir
#endif

using rainbow::czstring;
using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::SeekOrigin;
using rainbow::WriteableFile;

#define ASSERT_EMPTY_DATA(tmpfile, file_type)                                  \
    do                                                                         \
    {                                                                          \
        const auto& data = File::read(tmpfile.c_str(), file_type);             \
        ASSERT_FALSE(data);                                                    \
    } while (false)

#define ASSERT_EMPTY_FILE(tmpfile, file_type)                                  \
    do                                                                         \
    {                                                                          \
        const auto& file = File::open(tmpfile.c_str(), file_type);             \
        ASSERT_EQ(file.size(), 0u);                                            \
    } while (false)

namespace
{
    constexpr size_t kTestFileSuccess = 0xabad1dea;

    class TestFileImpl
    {
    protected:
        static constexpr bool open(czstring, FileType, TestFileImpl&)
        {
            return true;
        }

        static auto resolve_path(czstring path, FileType) -> std::string
        {
            return path;
        }

        constexpr TestFileImpl() {}
        TestFileImpl(File&&) noexcept {}

        auto handle() const -> FILE* { return nullptr; }
        void set_handle(FILE*) {}

        constexpr auto is_platform_handle() const { return true; }
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
    constexpr int max = std::numeric_limits<int>::max();
    const auto& tmpfile = std::to_string(rainbow::random(max / 2, max));

    {
        std::fclose(std::fopen(tmpfile.c_str(), "wb"));

        ASSERT_EMPTY_DATA(tmpfile, FileType::Asset);
        ASSERT_EMPTY_FILE(tmpfile, FileType::Asset);

        ASSERT_EQ(std::remove(tmpfile.c_str()), 0);
    }
    {
        namespace fs = rainbow::filesystem;

        std::error_code error_code;
        fs::create_directories(fs::user_data_path(), std::ref(error_code));

        ASSERT_TRUE(WriteableFile::open(tmpfile.c_str()));
        ASSERT_EMPTY_DATA(tmpfile, FileType::UserAsset);
        ASSERT_EMPTY_FILE(tmpfile, FileType::UserAsset);

        const auto& tmppath = fs::user(tmpfile.c_str());

        SCOPED_TRACE(tmppath.c_str());
        ASSERT_EQ(std::remove(tmppath.c_str()), 0);

        // Wait until the file has been removed...
        struct stat file_status;
        while (stat(tmppath.c_str(), &file_status) == 0) {}

        SCOPED_TRACE(fs::user_data_path());
        ASSERT_EQ(_rmdir(fs::user_data_path()), 0);
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
    ASSERT_EQ(writeable_file.read(nullptr, rainbow::kInvalidFileSize),
              kTestFileSuccess);
    ASSERT_EQ(writeable_file.write(nullptr, rainbow::kInvalidFileSize),
              kTestFileSuccess);
}

TEST(FileTypeTest, CorrespondsToFileAccessMode)
{
    ASSERT_STREQ(rainbow::detail::file_access_mode(FileType::Asset), "rb");
    ASSERT_STREQ(rainbow::detail::file_access_mode(FileType::UserAsset), "rb");
    ASSERT_STREQ(rainbow::detail::file_access_mode(FileType::UserFile), "w+b");
}

TEST(SeekOriginTest, CorrespondsToSeekOriginParameter)
{
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::Current), SEEK_CUR);
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::End), SEEK_END);
    ASSERT_EQ(rainbow::detail::seek_origin(SeekOrigin::Set), SEEK_SET);
}
