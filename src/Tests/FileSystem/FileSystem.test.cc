// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/FileSystem.h"

#include <cstring>

#include <gtest/gtest.h>

#include "Tests/TestHelpers.h"

#ifdef RAINBOW_OS_WINDOWS
#    include <direct.h>
#    define getcwd(buf, size) _getcwd(buf, size)
#    define kPathSeparator "\\"
#else
#    include <unistd.h>
#    define kPathSeparator "/"
#endif

namespace fs = rainbow::filesystem;
namespace sys = rainbow::system;

using rainbow::test::ScopedAssetsDirectory;

TEST(FileSystemTest, CreatesDirectories)
{
    ScopedAssetsDirectory scoped_assets{"FileSystemTest"};

    fs::Path path;
    path /= "1";
    path /= "2";
    path /= "3";
    path /= "4";

    ASSERT_EQ(path.string(), "1/2/3/4");
    ASSERT_TRUE(fs::create_directories(path));

    size_t offset = path.string().length();
    while (offset != std::string::npos)
    {
        auto p = path.string().substr(0, offset);

        ASSERT_TRUE(fs::is_directory(p.c_str()));
        ASSERT_TRUE(fs::remove(p.c_str()));
        ASSERT_FALSE(fs::is_directory(p.c_str()));

        offset = p.rfind('/', offset);
    }

    ASSERT_FALSE(fs::is_directory("1"));
}

TEST(FileSystemTest, FileExists)
{
    ScopedAssetsDirectory scoped_assets{"FileSystemTest"};

    ASSERT_FALSE(fs::exists("does not exist"));
    ASSERT_FALSE(fs::is_directory("does not exist"));
    ASSERT_FALSE(fs::is_regular_file("does not exist"));

    ASSERT_TRUE(fs::exists("empty.dat"));
    ASSERT_FALSE(fs::is_directory("empty.dat"));
    ASSERT_TRUE(fs::is_regular_file("empty.dat"));

    ASSERT_TRUE(fs::exists("folder"));
    ASSERT_TRUE(fs::is_directory("folder"));
    ASSERT_FALSE(fs::is_regular_file("folder"));
}

TEST(FileSystemTest, ReturnsPlatformPathSeparator)
{
    ASSERT_STREQ(fs::path_separator(), kPathSeparator);
}

TEST(FileSystemTest, ReturnsRealPath)
{
    ScopedAssetsDirectory scoped_assets{"FileSystemTest"};
    auto full_path = fs::real_path("empty.dat");

    ASSERT_EQ(full_path, scoped_assets.path() / "empty.dat");
}

TEST(FileSystemTest, SystemReturnsCurrentPath)
{
    char cwd[256];

    ASSERT_EQ(getcwd(cwd, sizeof(cwd)), cwd);
    ASSERT_EQ(sys::current_path(), cwd);
}

TEST(FileSystemTest, SystemReturnsWhetherPathIsDirectory)
{
    ScopedAssetsDirectory scoped_assets{"FileSystemTest"};

    ASSERT_TRUE(sys::is_directory(scoped_assets.c_str()));
    ASSERT_FALSE(sys::is_regular_file(scoped_assets.c_str()));
}

TEST(FileSystemTest, SystemReturnsWhetherPathIsRegularFile)
{
    ScopedAssetsDirectory scoped_assets{"FileSystemTest"};
    auto file_path = fs::real_path("empty.dat");

    ASSERT_FALSE(sys::is_directory(file_path.c_str()));
    ASSERT_TRUE(sys::is_regular_file(file_path.c_str()));
}
