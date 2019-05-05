// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Bundle.h"

#include <gtest/gtest.h>

#include "FileSystem/FileSystem.h"
#include "Tests/TestHelpers.h"

using rainbow::Bundle;
using rainbow::czstring;
using rainbow::zstring;
using rainbow::test::fixture_path;
using rainbow::test::ScopedAssetsDirectory;
using rainbow::test::ScopedFile;

namespace fs = rainbow::filesystem;
namespace sys = rainbow::system;

TEST(BundleTest, WithoutArguments)
{
    std::string executable = "rainbow";

    zstring args[1]{executable.data()};
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_EQ(bundle.assets_path(), cwd);
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), nullptr);
}

TEST(BundleTest, WithNonExistingPath)
{
    std::string executable = "rainbow";
    std::string a_path = "this_path_should_not_exist";

    zstring args[2]{executable.data(), a_path.data()};
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_EQ(bundle.assets_path(), cwd);
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), nullptr);
}

TEST(BundleTest, WithScriptDirectory)
{
    std::string executable = "rainbow";

    ScopedAssetsDirectory scoped_assets{"BundleTest_WithScriptDirectory"};

    zstring args[2]{
        executable.data(),
        const_cast<zstring>(scoped_assets.c_str()),
    };
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_STREQ(bundle.assets_path(), scoped_assets.c_str());
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), nullptr);
}

TEST(BundleTest, WithScriptInSameDirectory)
{
    std::string executable = "rainbow";
    std::string script = "this_script_should_not_exist.js";

    ScopedFile scoped_file{script};

    zstring args[2]{executable.data(), script.data()};
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_EQ(bundle.assets_path(), cwd);
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), script);
}

TEST(BundleTest, WithScriptInAnotherDirectory)
{
    std::string executable = "rainbow";
    auto directory = fixture_path("BundleTest_WithScriptInAnotherDirectory");
    std::string script = "main.js";
    std::string script_path = (directory / script).c_str();

    zstring args[2]{executable.data(), script_path.data()};
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_STREQ(bundle.assets_path(), directory.c_str());
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), script);
}

TEST(BundleTest, WithZipFile)
{
    std::string executable = "rainbow";
    auto directory = fixture_path("BundleTest_WithZipFile");
    std::string assets_path = (directory / "assets.zip").c_str();

    zstring args[2]{executable.data(), assets_path.data()};
    const Bundle bundle(args);
    auto cwd = sys::current_path();

    ASSERT_STREQ(bundle.assets_path(), assets_path.c_str());
    ASSERT_EQ(bundle.exec_path(), cwd + fs::path_separator() + executable);
    ASSERT_EQ(bundle.main_script(), nullptr);
}

TEST(BundleTest, IsMovable)
{
    std::string executable = "rainbow";
    auto directory = fixture_path("BundleTest_WithScriptInAnotherDirectory");
    std::string script_path = (directory / "main.js").c_str();

    zstring args[2]{executable.data(), script_path.data()};
    {
        Bundle bundle(args);
        czstring assets_path = bundle.assets_path();
        czstring exec_path = bundle.exec_path();
        czstring main_script = bundle.main_script();

        Bundle bundle2{std::move(bundle)};

        // Assert pointer equality
        ASSERT_EQ(bundle2.assets_path(), assets_path);
        ASSERT_EQ(bundle2.exec_path(), exec_path);
        ASSERT_EQ(bundle2.main_script(), main_script);
    }
    {
        Bundle bundle(args);
        czstring assets_path = bundle.assets_path();
        czstring exec_path = bundle.exec_path();
        czstring main_script = bundle.main_script();

        zstring args2[1]{executable.data()};
        Bundle bundle2(args2);

        // Assert pointer inequality
        ASSERT_NE(bundle2.assets_path(), assets_path);
        ASSERT_NE(bundle2.exec_path(), exec_path);
        ASSERT_NE(bundle2.main_script(), main_script);

        bundle2 = std::move(bundle);

        // Assert pointer equality
        ASSERT_EQ(bundle2.assets_path(), assets_path);
        ASSERT_EQ(bundle2.exec_path(), exec_path);
        ASSERT_EQ(bundle2.main_script(), main_script);
    }
}
