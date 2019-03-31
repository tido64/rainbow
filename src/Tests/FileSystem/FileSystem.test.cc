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

TEST(FileSystemTest, AssetsPath)
{
    char cwd[256];
    ASSERT_EQ(getcwd(cwd, sizeof(cwd)), cwd);
    ASSERT_STREQ(rainbow::filesystem::assets_path(), cwd);
}

TEST(FileSystemTest, CreatesDirectories)
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
        ASSERT_EQ(rmdir(p.c_str()), 0);
        offset = p.rfind(kPathSeparatorCharacter, offset);
    }
}

TEST(FileSystemTest, RelativeToCurrentPath)
{
    char cwd[256];
    ASSERT_EQ(getcwd(cwd, sizeof(cwd)), cwd);

    std::string path{cwd};
    path += kPathSeparatorCharacter;
    path += kTestFileName;

    ASSERT_STREQ(
        rainbow::filesystem::relative(kTestFileName).c_str(), path.c_str());
}

TEST(FileSystemTest, RelativeToRoot)
{
    ASSERT_STREQ(rainbow::filesystem::absolute(kTestPath).c_str(), kTestPath);
}
