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

using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::SeekOrigin;
using rainbow::WriteableFile;
using rainbow::czstring;

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
        ASSERT_EQ(0u, file.size());                                            \
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
}

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

        ASSERT_EQ(0, std::remove(tmpfile.c_str()));
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
        ASSERT_EQ(0, std::remove(tmppath.c_str()));

        // Wait until the file has been removed...
        struct stat file_status;
        while (stat(tmppath.c_str(), &file_status) == 0) {}

        SCOPED_TRACE(fs::user_data_path());
        ASSERT_EQ(0, _rmdir(fs::user_data_path()));
    }
}

TEST(FileTest, SupportsPlatformImplementation)
{
    const auto& file = TestFile::open("/", FileType::Asset);

    ASSERT_TRUE(file);
    ASSERT_TRUE(file.seek(rainbow::kInvalidFileSize));
    ASSERT_EQ(kTestFileSuccess, file.size());
    ASSERT_EQ(kTestFileSuccess, file.read(nullptr, rainbow::kInvalidFileSize));

    const auto& writeable_file = TestWriteableFile::open("/");

    ASSERT_TRUE(writeable_file);
    ASSERT_TRUE(writeable_file.seek(rainbow::kInvalidFileSize));
    ASSERT_EQ(kTestFileSuccess, writeable_file.size());
    ASSERT_EQ(kTestFileSuccess,  //
              writeable_file.read(nullptr, rainbow::kInvalidFileSize));
    ASSERT_EQ(kTestFileSuccess,  //
              writeable_file.write(nullptr, rainbow::kInvalidFileSize));
}

TEST(FileTypeTest, CorrespondsToFileAccessMode)
{
    ASSERT_STREQ("rb", rainbow::detail::file_access_mode(FileType::Asset));
    ASSERT_STREQ("rb", rainbow::detail::file_access_mode(FileType::UserAsset));
    ASSERT_STREQ("w+b", rainbow::detail::file_access_mode(FileType::UserFile));
}

TEST(SeekOriginTest, CorrespondsToSeekOriginParameter)
{
    ASSERT_EQ(SEEK_CUR, rainbow::detail::seek_origin(SeekOrigin::Current));
    ASSERT_EQ(SEEK_END, rainbow::detail::seek_origin(SeekOrigin::End));
    ASSERT_EQ(SEEK_SET, rainbow::detail::seek_origin(SeekOrigin::Set));
}
