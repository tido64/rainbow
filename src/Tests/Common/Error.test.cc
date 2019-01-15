// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Error.h"

#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "Common/Algorithm.h"
#include "Common/TypeCast.h"

using namespace std::literals::string_view_literals;

TEST(ErrorTest, IsSuccessByDefault)
{
    const auto error = rainbow::make_error_code({});

    ASSERT_STREQ(error.category().name(), "Rainbow");
    ASSERT_FALSE(error);
    ASSERT_TRUE(error.message().empty());
}

TEST(ErrorTest, HaveUniqueMessages)
{
    using rainbow::ErrorCode;

    std::vector<std::string> error_messages;
    error_messages.reserve(rainbow::to_underlying_type(ErrorCode::Unknown));

    for (auto i = rainbow::to_underlying_type(ErrorCode::Success) + 1;
         i <= rainbow::to_underlying_type(ErrorCode::Unknown);
         ++i)
    {
        const auto error = rainbow::make_error_code(static_cast<ErrorCode>(i));

        ASSERT_STREQ(error.category().name(), "Rainbow");
        ASSERT_TRUE(error);

        auto e = error.message();

        ASSERT_TRUE(std::none_of(
            error_messages.begin(), error_messages.end(), [&e](auto&& message) {
                return e == message;
            }));

        error_messages.emplace_back(std::move(e));
    }
}
