// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <string>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "FileSystem/Bundle.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

using rainbow::Bundle;
using rainbow::czstring;
using rainbow::zstring;
using rainbow::filesystem::Path;

namespace rainbow::filesystem::test
{
    auto bundle() -> const Bundle*;
}  // namespace rainbow::filesystem::test

namespace
{
    constexpr char kConfigTestPath[] =
        "rainbow-test-random-path-to-avoid-existing-config-files";

    constexpr char kAlternateConfig[] =
        "[core]\n"
        "ResolutionWidth = 750\n"
        "ResolutionHeight = 1334\n"
        "MSAA = 6\n"
        "AllowHiDPI = 0\n"
        "SuspendOnFocusLost = 1\n"
        "Accelerometer = 1\n";

    constexpr char kInvalidConfig[] = "💩";

    constexpr char kMissingValuesConfig[] =
        "[core]\n"
        "ResolutionWidth =\n"
        "ResolutionHeight =\n"
        "MSAA =\n"
        "AllowHiDPI =\n"
        "SuspendOnFocusLost =\n"
        "Accelerometer =\n";

    constexpr char kSparseConfig[] =
        "[core]\n"
        "MSAA = 10\n"
        "AllowHiDPI = true\n"
        "SuspendOnFocusLost = false\n";

    constexpr char kStandardConfig[] =
        "[core]\n"
        "ResolutionWidth = 1920\n"
        "ResolutionHeight = 1080\n"
        "MSAA = 4\n"
        "AllowHiDPI = true\n"
        "SuspendOnFocusLost = false\n"
        "Accelerometer = false\n";

    class ScopedAssetsDirectory
    {
    public:
        explicit ScopedAssetsDirectory(czstring path)
            : current_bundle_(rainbow::filesystem::test::bundle()),
              scope_bundle_(current_bundle_->exec_path(), path)
        {
            rainbow::filesystem::initialize(scope_bundle_);
        }

        ~ScopedAssetsDirectory()
        {
            rainbow::filesystem::initialize(*current_bundle_);
        }

    private:
        const Bundle* current_bundle_;
        Bundle scope_bundle_;
    };

    class ScopedConfig
    {
    public:
        template <size_t N>
        explicit ScopedConfig(const char (&config)[N])
            : ScopedConfig(config, N - 1)
        {
        }

        ~ScopedConfig()
        {
            std::error_code error;
            rainbow::filesystem::remove(path_.c_str(), error);
            rainbow::filesystem::remove(kConfigTestPath, error);
        }

    private:
        Path path_;
        ScopedAssetsDirectory assets_path_;

        ScopedConfig(czstring config, size_t length)
            : path_(rainbow::filesystem::relative(kConfigTestPath)),
              assets_path_(kConfigTestPath)
        {
            std::error_code error;
            rainbow::filesystem::create_directories(path_, error);
            path_ /= "config.ini";
            FILE* fd = fopen(path_.c_str(), "wb");
            [fd] { ASSERT_NE(fd, nullptr); }();
            fwrite(config, sizeof(*config), length, fd);
            fclose(fd);
        }
    };
}  // namespace

TEST(ConfigTest, NoConfiguration)
{
    {
        ScopedConfig clear{kInvalidConfig};
    }

    ScopedAssetsDirectory test(kConfigTestPath);
    rainbow::Config config;

    ASSERT_EQ(config.width(), 0);
    ASSERT_EQ(config.height(), 0);
    ASSERT_FALSE(config.hidpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(config.msaa(), 0u);
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, EmptyConfiguration)
{
    ScopedConfig conf("");
    rainbow::Config config;

    ASSERT_EQ(config.width(), 0);
    ASSERT_EQ(config.height(), 0);
    ASSERT_FALSE(config.hidpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(config.msaa(), 0u);
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, InvalidConfiguration)
{
    ScopedConfig conf(kInvalidConfig);
    rainbow::Config config;

    ASSERT_EQ(config.width(), 0);
    ASSERT_EQ(config.height(), 0);
    ASSERT_FALSE(config.hidpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(config.msaa(), 0u);
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, NormalConfiguration)
{
    ScopedConfig config(kStandardConfig);
    rainbow::Config c;

    ASSERT_EQ(c.width(), 1920);
    ASSERT_EQ(c.height(), 1080);
    ASSERT_TRUE(c.hidpi());
    ASSERT_FALSE(c.is_portrait());
    ASSERT_EQ(c.msaa(), 4u);
    ASSERT_FALSE(c.needs_accelerometer());
    ASSERT_FALSE(c.suspend());
}

TEST(ConfigTest, AlternateConfiguration)
{
    ScopedConfig config(kAlternateConfig);
    rainbow::Config c;

    ASSERT_EQ(c.width(), 750);
    ASSERT_EQ(c.height(), 1334);
    ASSERT_FALSE(c.hidpi());
    ASSERT_TRUE(c.is_portrait());
    ASSERT_EQ(c.msaa(), 4u);
    ASSERT_TRUE(c.needs_accelerometer());
    ASSERT_TRUE(c.suspend());
}

TEST(ConfigTest, SparseConfiguration)
{
    ScopedConfig config(kSparseConfig);
    rainbow::Config c;

    ASSERT_EQ(c.width(), 0);
    ASSERT_EQ(c.height(), 0);
    ASSERT_TRUE(c.hidpi());
    ASSERT_FALSE(c.is_portrait());
    ASSERT_EQ(c.msaa(), 8u);
    ASSERT_FALSE(c.suspend());
}

TEST(ConfigTest, MissingValues)
{
    ScopedConfig conf(kMissingValuesConfig);
    rainbow::Config config;

    ASSERT_EQ(config.width(), 0);
    ASSERT_EQ(config.height(), 0);
    ASSERT_FALSE(config.hidpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(config.msaa(), 0u);
    ASSERT_TRUE(config.suspend());
}
