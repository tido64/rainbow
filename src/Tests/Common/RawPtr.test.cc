// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/RawPtr.h"

using rainbow::RawPtr;

namespace
{
    constexpr uint32_t kMax = 0xffffffffu;
}

TEST(RawPtrTest, ConcatenatesUpperLowerBits)
{
    {
        RawPtr<void> high{kMax, 0u};

        ASSERT_EQ(kMax, high.upper());
        ASSERT_EQ(0u, high.lower());
        ASSERT_EQ(reinterpret_cast<void*>(0xffffffff00000000), high.get());
    }
    {
        RawPtr<void> low{0u, kMax};

        ASSERT_EQ(0u, low.upper());
        ASSERT_EQ(kMax, low.lower());
        ASSERT_EQ(reinterpret_cast<void*>(0x00000000ffffffff), low.get());
    }
}

TEST(RawPtrTest, SplitsUpperLowerBits)
{
    {
        auto p = reinterpret_cast<void*>(0xffffffff00000000);
        RawPtr<void> high{p};

        ASSERT_EQ(p, high.get());
        ASSERT_EQ(kMax, high.upper());
        ASSERT_EQ(0u, high.lower());
    }
    {
        auto p = reinterpret_cast<void*>(0x00000000ffffffff);
        RawPtr<void> low{p};

        ASSERT_EQ(p, low.get());
        ASSERT_EQ(0u, low.upper());
        ASSERT_EQ(kMax, low.lower());
    }
}
