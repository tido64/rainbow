// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/String.h"
#include "FileSystem/KnownPaths.h"

TEST(KnownPathsTest, GetsExecutablePath)
{
    constexpr char kExecutablePath[] = "/rainbow";

    ASSERT_TRUE(rainbow::ends_with(
        rainbow::filesystem::get_executable_path(), kExecutablePath));
}

TEST(KnownPathsTest, GetsSaveDirectory)
{
    constexpr char kApplicationSupport[] = "/Library/Application Support";

    ASSERT_TRUE(rainbow::ends_with(
        rainbow::filesystem::get_save_directory(), kApplicationSupport));
}
