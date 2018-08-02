// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <string>

#include <gtest/gtest.h>

#include "Memory/ArrayMap.h"

#define KEY_PREFIX                                                             \
    "rainbow://some-really-long-key-to-prevent-small-string-optimization-"

using rainbow::ArrayMap;

namespace
{
    constexpr const char* const kTestKeys[]{
        KEY_PREFIX "one",
        KEY_PREFIX "two",
        KEY_PREFIX "three",
        KEY_PREFIX "four",
        KEY_PREFIX "five",
        KEY_PREFIX "six",
        KEY_PREFIX "seven",
        KEY_PREFIX "eight",
        KEY_PREFIX "nine",
        KEY_PREFIX "ten",
    };

    class ArrayMapTest : public ::testing::Test
    {
    protected:
        ArrayMap<std::string, int> map_;

        void SetUp() override
        {
            for (int i = 0; i < 10; ++i)
                map_.emplace(kTestKeys[i], i + 1);
        }
    };
}  // namespace

TEST_F(ArrayMapTest, ReturnsLastElement)
{
    ASSERT_EQ(map_.back(), 10);

    map_.emplace(KEY_PREFIX "eleven", 11);

    ASSERT_EQ(map_.back(), 11);
}

TEST_F(ArrayMapTest, ReturnsIterators)
{
    auto i = map_.begin();

    ASSERT_EQ(i->first, KEY_PREFIX "one");
    ASSERT_EQ(i->second, 1);

    i += map_.size();

    ASSERT_EQ(i, map_.end());
}

TEST_F(ArrayMapTest, ClearsMap)
{
    ASSERT_FALSE(map_.empty());

    map_.clear();

    ASSERT_TRUE(map_.empty());
}

TEST_F(ArrayMapTest, FindsKey)
{
    for (int i = 0; i < 10; ++i)
    {
        auto j = map_.find(kTestKeys[i]);

        ASSERT_NE(j, map_.end());
        ASSERT_EQ(j->first, kTestKeys[i]);
        ASSERT_EQ(j->second, i + 1);
    }

    for (int i = 0; i < 10; ++i)
    {
        std::string_view key{kTestKeys[i]};
        auto j = map_.find(key);

        ASSERT_NE(j, map_.end());
        ASSERT_EQ(j->first, key);
        ASSERT_EQ(j->second, i + 1);
    }

    ASSERT_EQ(map_.find(""), map_.end());
}

TEST_F(ArrayMapTest, FindsValue)
{
    for (int i = 0; i < 10; ++i)
    {
        auto j = map_.find_value(i + 1);

        ASSERT_NE(j, map_.end());
        ASSERT_EQ(j->first, kTestKeys[i]);
        ASSERT_EQ(j->second, i + 1);
    }

    ASSERT_EQ(map_.find_value(0), map_.end());
}

TEST_F(ArrayMapTest, EmplaceDoesNotReplace)
{
    auto result = map_.emplace(KEY_PREFIX "seven", 42);

    ASSERT_FALSE(result.second);
    ASSERT_EQ(result.first->second, 7);
}

TEST_F(ArrayMapTest, ErasesKey)
{
    auto seven = map_.find(KEY_PREFIX "seven");

    ASSERT_NE(seven, map_.end());

    map_.erase(KEY_PREFIX "seven");
    seven = map_.find(KEY_PREFIX "seven");

    ASSERT_EQ(seven, map_.end());
}

TEST_F(ArrayMapTest, ErasesNonExistingKey)
{
    ASSERT_EQ(map_.size(), 10u);

    map_.erase("");

    ASSERT_EQ(map_.size(), 10u);
}

TEST_F(ArrayMapTest, ErasesRange)
{
    ASSERT_EQ(map_.size(), 10u);

    map_.erase(std::begin(map_), std::begin(map_) + 5);

    ASSERT_EQ(map_.size(), 5u);

    for (int i = 5; i < 10; ++i)
    {
        auto j = map_.find(kTestKeys[i]);

        ASSERT_NE(j, map_.end());
        ASSERT_EQ(j->first, kTestKeys[i]);
        ASSERT_EQ(j->second, i + 1);
    }

    map_.erase(std::begin(map_), std::end(map_));

    ASSERT_TRUE(map_.empty());
}

TEST_F(ArrayMapTest, ReturnsValueForIndex)
{
    for (int i = 0; i < 10; ++i)
        ASSERT_EQ(map_[kTestKeys[i]], i + 1);
}
