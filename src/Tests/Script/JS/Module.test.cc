// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/Module.h"

#include <gtest/gtest.h>

#include "Script/JavaScript/JavaScript.h"

namespace
{
    namespace duk = rainbow::duk;

    class JSModuleTest : public ::testing::Test
    {
    protected:
        duk::Context context_;
    };
}  // namespace

TEST_F(JSModuleTest, ResolvesMainModuleRelativePaths)
{
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "");
    duk::module::resolve(context_);
    auto result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested.js");
    duk_push_literal(context_, "");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "grandparent/parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "requested.js");
}

TEST_F(JSModuleTest, ResolvesRelativePaths)
{
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "");
    duk::module::resolve(context_);
    auto result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested.js");
    duk_push_literal(context_, "");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "../requested");
    duk_push_literal(context_, "");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "../requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "./parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "./grandparent/parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./grandparent/requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "../grandparent/parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "../grandparent/requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "../requested");
    duk_push_literal(context_, "./grandparent/parent");
    duk::module::resolve(context_);
    result = duk_get_string(context_, 2);

    ASSERT_STREQ(result, "./requested.js");
}
