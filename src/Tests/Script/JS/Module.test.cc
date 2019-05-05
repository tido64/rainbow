// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/JavaScript/Module.h"

#include <gtest/gtest.h>

#include "Tests/TestHelpers.h"

#define JOIN2(a, b) a "/" b
#define JOIN3(a, b, c) a "/" b "/" c

namespace
{
    namespace duk = rainbow::duk;

    void on_fatal(void*, const char* msg) { throw std::runtime_error(msg); }

    class JSModuleTest : public ::testing::Test
    {
    public:
        JSModuleTest()
            : context_(
                  duk_create_heap(nullptr, nullptr, nullptr, this, &on_fatal))
        {
        }

        ~JSModuleTest() { duk_destroy_heap(context_); }

    protected:
        duk_context* context_;
    };
}  // namespace

TEST_F(JSModuleTest, Initializes)
{
    duk::module::initialize(context_);
    ASSERT_EQ(duk_get_top(context_), 0);
}

TEST_F(JSModuleTest, ThrowsLoadingEmptyModuleName)
{
    duk_push_literal(context_, "");

    ASSERT_THROW(duk::module::load(context_), std::runtime_error);
    ASSERT_TRUE(duk_is_error(context_, 1));
}

TEST_F(JSModuleTest, LoadsModules)
{
    rainbow::test::ScopedAssetsDirectory scoped_assets(
        "JSModuleTest_LoadsModules");

    duk_push_literal(context_, "module.js");
    duk::module::load(context_);

    ASSERT_STREQ(duk_get_string(context_, 1), "\"use strict\";");
}

TEST_F(JSModuleTest, ThrowsResolvingEmptyModuleName)
{
    duk_push_literal(context_, "");
    duk_push_literal(context_, "");

    ASSERT_THROW(duk::module::resolve(context_), std::runtime_error);
    ASSERT_TRUE(duk_is_error(context_, 2));
}

TEST_F(JSModuleTest, ResolvesMainModuleRelativePaths)
{
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested.js");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), "requested.js");

    duk_set_top(context_, 0);
    duk_push_literal(context_, "requested");
    duk_push_literal(context_, "grandparent/parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), "requested.js");
}

TEST_F(JSModuleTest, ResolvesRelativePaths)
{
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested.js");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "../requested");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2("..", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "./parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "./grandparent/parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(
        duk_get_string(context_, 2), JOIN3(".", "grandparent", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "./requested");
    duk_push_literal(context_, "../grandparent/parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2),
                 JOIN3("..", "grandparent", "requested.js"));

    duk_set_top(context_, 0);
    duk_push_literal(context_, "../requested");
    duk_push_literal(context_, "./grandparent/parent");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.js"));
}

TEST_F(JSModuleTest, PreservesExtensions)
{
    duk_push_literal(context_, "./requested.base");
    duk_push_literal(context_, "");

    ASSERT_EQ(duk::module::resolve(context_), 1);
    ASSERT_STREQ(duk_get_string(context_, 2), JOIN2(".", "requested.base.js"));
}
