// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/String.h"

#include <gtest/gtest.h>

#define TEST_SUFFIX ".h"
#define TEST_STRING "String" TEST_SUFFIX
#define TEST_STRINGL "string" TEST_SUFFIX
#define WRONG_LONG "sequence" TEST_SUFFIX
#define WRONG_SHORT "cord" TEST_SUFFIX

using rainbow::StringComparison;

TEST(StringTest, ComparesStringEndings)
{
    const std::string str{TEST_STRING};

    ASSERT_TRUE(rainbow::ends_with(str, TEST_SUFFIX));
    ASSERT_TRUE(rainbow::ends_with(str, TEST_STRING));
    ASSERT_FALSE(rainbow::ends_with(str, TEST_STRINGL));
    ASSERT_FALSE(rainbow::ends_with(str, WRONG_LONG));
    ASSERT_FALSE(rainbow::ends_with(str, WRONG_SHORT));

    ASSERT_TRUE(rainbow::ends_with(str.c_str(), TEST_SUFFIX));
    ASSERT_TRUE(rainbow::ends_with(str.c_str(), TEST_STRING));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), TEST_STRINGL));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), WRONG_LONG));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), WRONG_SHORT));

    ASSERT_TRUE(rainbow::ends_with(
        str.c_str(), TEST_SUFFIX, StringComparison::Ordinal));
    ASSERT_TRUE(rainbow::ends_with(
        str.c_str(), TEST_STRING, StringComparison::Ordinal));
    ASSERT_FALSE(rainbow::ends_with(
        str.c_str(), TEST_STRINGL, StringComparison::Ordinal));
    ASSERT_FALSE(
        rainbow::ends_with(str.c_str(), WRONG_LONG, StringComparison::Ordinal));
    ASSERT_FALSE(rainbow::ends_with(
        str.c_str(), WRONG_SHORT, StringComparison::Ordinal));

    ASSERT_TRUE(rainbow::ends_with(
        str.c_str(), TEST_SUFFIX, StringComparison::IgnoreCase));
    ASSERT_TRUE(rainbow::ends_with(
        str.c_str(), TEST_STRING, StringComparison::IgnoreCase));
    ASSERT_TRUE(rainbow::ends_with(
        str.c_str(), TEST_STRINGL, StringComparison::IgnoreCase));
    ASSERT_FALSE(rainbow::ends_with(
        str.c_str(), WRONG_LONG, StringComparison::IgnoreCase));
    ASSERT_FALSE(rainbow::ends_with(
        str.c_str(), WRONG_SHORT, StringComparison::IgnoreCase));

    ASSERT_TRUE(rainbow::ends_with(str, TEST_SUFFIX));
    ASSERT_TRUE(rainbow::ends_with(str, TEST_STRING));
    ASSERT_FALSE(rainbow::ends_with(str, TEST_STRINGL));
    ASSERT_FALSE(rainbow::ends_with(str, WRONG_LONG));
    ASSERT_FALSE(rainbow::ends_with(str, WRONG_SHORT));
}

TEST(StringTest, ReturnsWhetherStringIsEmpty)
{
    ASSERT_TRUE(rainbow::is_empty(nullptr));
    ASSERT_TRUE(rainbow::is_empty(""));
    ASSERT_TRUE(rainbow::is_empty("\0"));
    ASSERT_FALSE(rainbow::is_empty("🌈"));
}
