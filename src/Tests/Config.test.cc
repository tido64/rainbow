// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <gtest/gtest.h>

#include "Tests/TestHelpers.h"

using rainbow::test::ScopedAssetsDirectory;

TEST(ConfigTest, NoConfiguration)
{
    ScopedAssetsDirectory scoped_assets("ConfigTest_NoConfiguration");
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_EmptyConfiguration"};
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_InvalidConfiguration"};
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_NormalConfiguration"};
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_AlternateConfiguration"};
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_SparseConfiguration"};
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
    ScopedAssetsDirectory scoped_assets{"ConfigTest_MissingValues"};
    rainbow::Config config;

    ASSERT_EQ(config.width(), 0);
    ASSERT_EQ(config.height(), 0);
    ASSERT_FALSE(config.hidpi());
    ASSERT_FALSE(config.is_portrait());
    ASSERT_EQ(config.msaa(), 0u);
    ASSERT_TRUE(config.suspend());
}
