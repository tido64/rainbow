// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Common/Variant.h"

namespace
{
    template <typename T>
    class TestVisitor
    {
    public:
        TestVisitor(T value) : value_(value) {}

        void operator()(T value) const { ASSERT_EQ(value_, value); }

        template <typename U>
        void operator()(U&&) const
        {
            FAIL();
        }

    private:
        T value_;
    };
}  // namespace

TEST(VariantTest, ReturnsReferenceToValue)
{
    const rainbow::variant<void*, uint32_t> v{this};

    ASSERT_TRUE(rainbow::holds_alternative<void*>(v));
    ASSERT_EQ(this, rainbow::get<void*>(v).value_or(nullptr));
    ASSERT_FALSE(rainbow::holds_alternative<uint32_t>(v));
    ASSERT_EQ(std::nullopt, rainbow::get<uint32_t>(v));

    rainbow::variant<void*, uint32_t> w{1};

    ASSERT_FALSE(rainbow::holds_alternative<void*>(w));
    ASSERT_EQ(std::nullopt, rainbow::get<void*>(w));
    ASSERT_TRUE(rainbow::holds_alternative<uint32_t>(w));
    ASSERT_EQ(1u, rainbow::get<uint32_t>(w).value_or(0xabad1dea));
}

TEST(VariantTest, AppliesVisitor)
{
    rainbow::visit(TestVisitor<void*>{this},  //
                   rainbow::variant<void*, uint32_t>{this});
    rainbow::visit(TestVisitor<uint32_t>{0xabad1dea},
                   rainbow::variant<void*, uint32_t>{0xabad1dea});
}
