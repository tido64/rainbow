// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "FileSystem/Bundle.h"
#include "FileSystem/FileSystem.h"
#include "Tests/TestHelpers.h"

using rainbow::Bundle;
using rainbow::test::ScopedDirectory;
using rainbow::test::ScopedFile;
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
    std::string directory = "this_path_should_not_exist";

    ScopedDirectory a_directory{directory};

    zstring args[2]{executable.data(), directory.data()};
    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(directory, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(nullptr, bundle.main_script());
}

TEST(BundleTest, WithScriptInSameDirectory)
{
    std::string executable = "rainbow";
    std::string script = "this_script_should_not_exist.js";

    ScopedFile a_file{script};

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
    std::string directory = "this_path_should_not_exist";
    std::string script = "index.js";
    std::string script_path = directory + fs::path_separator() + script;

    ScopedDirectory a_directory{directory};
    ScopedFile a_file{script_path};

    zstring args[2]{executable.data(), script_path.data()};
    Bundle bundle{args};
    auto cwd = fs::system_current_path();

    ASSERT_EQ(directory, bundle.assets_path());
    ASSERT_EQ(cwd + fs::path_separator() + executable, bundle.exec_path());
    ASSERT_EQ(script, bundle.main_script());
}
