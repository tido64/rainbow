// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"

using rainbow::Bundle;
using rainbow::zstring;

namespace fs = rainbow::filesystem;

namespace rainbow::filesystem::test
{
    auto bundle() -> const Bundle*;
}  // namespace rainbow::filesystem::test

TEST(BundleTest, WithoutArguments)
{
    std::string executable = "rainbow";
    zstring args[1]{executable.data()};
    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(cwd, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(nullptr, bundle.main_script());
}

TEST(BundleTest, WithNonExistingPath)
{
    std::string executable = "rainbow";
    std::string a_path = "this_path_should_not_exist";
    zstring args[2]{executable.data(), a_path.data()};

    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(cwd, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(nullptr, bundle.main_script());
}

TEST(BundleTest, WithScriptDirectory)
{
    std::string executable = "rainbow";
    std::string directory = "app";

    std::error_code error;
    fs::create_directories(directory.c_str(), std::ref(error));

    zstring args[2]{executable.data(), directory.data()};

    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(directory, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(nullptr, bundle.main_script());

    fs::remove(directory.c_str(), error);
}

TEST(BundleTest, WithScriptInSameDirectory)
{
    std::string executable = "rainbow";
    std::string script = executable;
    zstring args[2]{executable.data(), script.data()};

    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(cwd, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(script, bundle.main_script());
}

TEST(BundleTest, WithScriptInAnotherDirectory)
{
    std::string executable = "rainbow";
    std::string directory = "app";
    std::string script = "index.js";
    std::string script_path = directory + fs::path_separator() + script;

    std::error_code error;
    fs::create_directories(directory.c_str(), std::ref(error));
    FILE* fd = fopen(script_path.c_str(), "wb");
    ASSERT_NE(nullptr, fd);
    fwrite(script_path.data(), 1, script_path.length(), fd);
    fclose(fd);

    zstring args[2]{executable.data(), script_path.data()};

    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(directory, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(script, bundle.main_script());

    fs::remove(script_path.c_str(), error);
    fs::remove(directory.c_str(), error);
}
