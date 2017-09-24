// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include <Rainbow/TextAlignment.h>

TEST(TextAlignmentTest, FromChar)
{
    for (int c = 0; c < 128; ++c)
    {
        const auto alignment =
            rainbow::text_alignment_from_char(static_cast<char>(c));
        switch (c)
        {
            case 'c':
                ASSERT_EQ(rainbow::TextAlignment::Center, alignment);
                break;
            case 'r':
                ASSERT_EQ(rainbow::TextAlignment::Right, alignment);
                break;
            default:
                ASSERT_EQ(rainbow::TextAlignment::Left, alignment);
                break;
        }
    }
}
