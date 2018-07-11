// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <string>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Config.h"
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
        "{"
        "  \"accelerometer\": true,"
        "  \"allowHighDPI\": false,"
        "  \"msaa\": 6,"
        "  \"resolution\": {"
        "    \"width\": 750,"
        "    \"height\": 1334"
        "  },"
        "  \"suspendOnFocusLost\": true"
        "}";

    constexpr char kInvalidConfig[] = "💩";

    constexpr char kSparseConfig[] =
        "{"
        "  \"allowHighDPI\": true,"
        "  \"msaa\": 10,"
        "  \"suspendOnFocusLost\": false"
        "}";

    constexpr char kStandardConfig[] =
        "{"
        "  \"accelerometer\": false,"
        "  \"allowHighDPI\": true,"
        "  \"msaa\": 4,"
        "  \"resolution\": {"
        "    \"width\": 1920,"
        "    \"height\": 1080"
        "  },"
        "  \"suspendOnFocusLost\": false"
        "}";

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
            path_ /= "config.json";
            FILE* fd = fopen(path_.c_str(), "wb");
            [fd] { ASSERT_NE(nullptr, fd); }();
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

    ASSERT_EQ(0, config.width());
    ASSERT_EQ(0, config.height());
    ASSERT_FALSE(config.high_dpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(0u, config.msaa());
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, EmptyConfiguration)
{
    ScopedConfig conf("");
    rainbow::Config config;

    ASSERT_EQ(0, config.width());
    ASSERT_EQ(0, config.height());
    ASSERT_FALSE(config.high_dpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(0u, config.msaa());
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, InvalidConfiguration)
{
    ScopedConfig conf(kInvalidConfig);
    rainbow::Config config;

    ASSERT_EQ(0, config.width());
    ASSERT_EQ(0, config.height());
    ASSERT_FALSE(config.high_dpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(0u, config.msaa());
    ASSERT_TRUE(config.suspend());
}

TEST(ConfigTest, NormalConfiguration)
{
    ScopedConfig config(kStandardConfig);
    rainbow::Config c;

    ASSERT_EQ(1920, c.width());
    ASSERT_EQ(1080, c.height());
    ASSERT_TRUE(c.high_dpi());
    ASSERT_FALSE(c.is_portrait());
    ASSERT_EQ(4u, c.msaa());
    ASSERT_FALSE(c.needs_accelerometer());
    ASSERT_FALSE(c.suspend());
}

TEST(ConfigTest, AlternateConfiguration)
{
    ScopedConfig config(kAlternateConfig);
    rainbow::Config c;

    ASSERT_EQ(750, c.width());
    ASSERT_EQ(1334, c.height());
    ASSERT_FALSE(c.high_dpi());
    ASSERT_TRUE(c.is_portrait());
    ASSERT_EQ(4u, c.msaa());
    ASSERT_TRUE(c.needs_accelerometer());
    ASSERT_TRUE(c.suspend());
}

TEST(ConfigTest, SparseConfiguration)
{
    ScopedConfig config(kSparseConfig);
    rainbow::Config c;

    ASSERT_EQ(0, c.width());
    ASSERT_EQ(0, c.height());
    ASSERT_TRUE(c.high_dpi());
    ASSERT_FALSE(c.is_portrait());
    ASSERT_EQ(8u, c.msaa());
    ASSERT_FALSE(c.suspend());
}
