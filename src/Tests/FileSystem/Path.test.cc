// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>

#include <gtest/gtest.h>

#include "Common/Random.h"
#include "FileSystem/FileSystem.h"

#ifdef RAINBOW_OS_WINDOWS
#   include <cstdint>
#   include <direct.h>
#   define getcwd(buf, size) _getcwd(buf, size)
#   define kPathSeparator           "\\"
#   define kPathSeparatorCharacter  '\\'
#   define rmdir(path) _rmdir(path)
using mode_t = uint16_t;
#else
#   include <unistd.h>
#   define kPathSeparator           "/"
#   define kPathSeparatorCharacter  '/'
#endif
#define kTestFileName    "rainbow.file"
#define kTestPath        "rainbow-test"
#define kTestRandomPath  "rainbow-%x"

// For tests of File, see DataTest::SaveAndLoad.

using rainbow::filesystem::Path;

TEST(PathTest, CreatesDirectories)
{
    char random[32];
    snprintf(random,
             sizeof(random),
             kTestRandomPath,
             rainbow::random(0x10000000u, 0xffffffffu));

    std::error_code error;
    Path path = rainbow::filesystem::absolute(random);
    path /= "1";
    path /= "2";
    path /= "3";
    path /= "4";

    ASSERT_TRUE(rainbow::filesystem::create_directories(path, error));

    size_t offset = path.string().length();
    while (offset != std::string::npos)
    {
        auto p = path.string().substr(0, offset);
        ASSERT_TRUE(rainbow::filesystem::is_directory(p.c_str(), error));
        ASSERT_EQ(0, rmdir(p.c_str()));
        offset = p.rfind(kPathSeparatorCharacter, offset);
    }
}

TEST(PathTest, CurrentPath)
{
    char cwd[256];
    ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));
    ASSERT_STREQ(cwd, rainbow::filesystem::current_path());
}

TEST(PathTest, RelativeToCurrentPath)
{
    char cwd[256];
    ASSERT_EQ(cwd, getcwd(cwd, sizeof(cwd)));

    std::string path{cwd};
    path += kPathSeparatorCharacter;
    path += kTestFileName;

    ASSERT_STREQ(
        path.c_str(), rainbow::filesystem::relative(kTestFileName).c_str());
}

TEST(PathTest, RelativeToRoot)
{
    ASSERT_STREQ(kTestPath, rainbow::filesystem::absolute(kTestPath).c_str());
}

TEST(PathTest, SetsPath)
{
    Path path;
    ASSERT_STRNE(kTestPath, path.c_str());
    path = kTestPath;
    ASSERT_STREQ(kTestPath, path.c_str());
}

TEST(PathTest, AppendsPathComponents)
{
    Path path;
    path = kTestPath;
    ASSERT_STREQ(kTestPath, path.c_str());
    path /= kTestFileName;
    ASSERT_STREQ(kTestPath kPathSeparator kTestFileName, path.c_str());
    path /= kPathSeparator;
    ASSERT_STREQ(
        kTestPath kPathSeparator kTestFileName kPathSeparator kPathSeparator,
        path.c_str());
    path /= kTestFileName;
    ASSERT_STREQ(kTestPath kPathSeparator kTestFileName kPathSeparator
                     kPathSeparator kTestFileName,
                 path.c_str());
}
