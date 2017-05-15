// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/String.h"

#define TEST_SUFFIX ".h"
#define TEST_STRING "String" TEST_SUFFIX
#define TEST_STRINGL "string" TEST_SUFFIX
#define WRONG_LONG "sequence" TEST_SUFFIX
#define WRONG_SHORT "cord" TEST_SUFFIX

using rainbow::StringComparison;

TEST(StringTest, ComparesStringEndings)
{
    const std::string str{TEST_STRING};

    ASSERT_TRUE(rainbow::ends_with(str.c_str(), str.length(), TEST_SUFFIX));
    ASSERT_TRUE(rainbow::ends_with(str.c_str(), str.length(), TEST_STRING));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), str.length(), TEST_STRINGL));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), str.length(), WRONG_LONG));
    ASSERT_FALSE(rainbow::ends_with(str.c_str(), str.length(), WRONG_SHORT));

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
    ASSERT_FALSE(rainbow::ends_with(
        str.c_str(), WRONG_LONG, StringComparison::Ordinal));
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
