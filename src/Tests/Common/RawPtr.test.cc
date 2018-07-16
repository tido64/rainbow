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

        ASSERT_EQ(high.upper(), kMax);
        ASSERT_EQ(high.lower(), 0u);
        ASSERT_EQ(high.get(), reinterpret_cast<void*>(0xffffffff00000000ull));
    }
    {
        RawPtr<void> low{0u, kMax};

        ASSERT_EQ(low.upper(), 0u);
        ASSERT_EQ(low.lower(), kMax);
        ASSERT_EQ(low.get(), reinterpret_cast<void*>(0x00000000ffffffffull));
    }
}

TEST(RawPtrTest, SplitsUpperLowerBits)
{
    {
        auto p = reinterpret_cast<void*>(0xffffffff00000000ull);
        RawPtr<void> high{p};

        ASSERT_EQ(high.get(), p);
        ASSERT_EQ(high.upper(), kMax);
        ASSERT_EQ(high.lower(), 0u);
    }
    {
        auto p = reinterpret_cast<void*>(0x00000000ffffffffull);
        RawPtr<void> low{p};

        ASSERT_EQ(low.get(), p);
        ASSERT_EQ(low.upper(), 0u);
        ASSERT_EQ(low.lower(), kMax);
    }
}
