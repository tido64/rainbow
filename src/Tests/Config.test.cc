// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <string>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Config.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

using rainbow::filesystem::Path;

namespace
{
    constexpr const char kConfigTestPath[] =
        "rainbow-test-random-path-to-avoid-existing-config-files";

    constexpr const char kAlternateConfig[] =
        "accelerometer = true;\n"
        "allow_high_dpi = false;\n"
        "msaa = 6;\n"
        "resolution = {750,1334};\n"
        "suspend_on_focus_lost = true;\n";

    constexpr const char kInvalidConfig[] = "@";

    constexpr const char kSparseConfig[] =
        "allow_high_dpi = true;\n"
        "msaa = 10;\n"
        "suspend_on_focus_lost = false;\n";

    constexpr const char kStandardConfig[] =
        "accelerometer = false;\n"
        "allow_high_dpi = true;\n"
        "msaa = 4;\n"
        "resolution = {1920,1080};\n"
        "suspend_on_focus_lost = false;\n";

    class ScopedCurrentDirectory
    {
    public:
        explicit ScopedCurrentDirectory(const char* path)
        {
            working_dir_ = rainbow::filesystem::current_path();
            rainbow::filesystem::set_current_path(path);
        }

        ~ScopedCurrentDirectory()
        {
            rainbow::filesystem::set_current_path(working_dir_.c_str());
        }

    private:
        std::string working_dir_;
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
        ScopedCurrentDirectory working_directory_;

        ScopedConfig(const char* config, size_t length)
            : path_(rainbow::filesystem::relative(kConfigTestPath)),
              working_directory_(kConfigTestPath)
        {
            std::error_code error;
            rainbow::filesystem::create_directories(path_, error);
            path_ /= "config";
            FILE* fd = fopen(path_.c_str(), "wb");
            [fd] { ASSERT_NE(nullptr, fd); }();
            fwrite(config, sizeof(*config), length, fd);
            fclose(fd);
        }
    };
}

TEST(ConfigTest, NoConfiguration)
{
    ScopedCurrentDirectory test(kConfigTestPath);
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
