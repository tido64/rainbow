// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <gtest/gtest.h>

#include "Lua/LuaHelper.h"

namespace lua = rainbow::lua;

namespace
{
    class LuaHelperTest : public ::testing::Test
    {
    protected:
        auto state() { return state_.get(); }

    private:
        std::unique_ptr<lua_State> state_;

        void SetUp() final
        {
            state_ = lua::newstate();
            lua_pushnil(state());                     // 1
            lua::push(state(), true);                 // 2
            lua::push(state(), 42);                   // 3
            lua::push(state(), 3.14f);                // 4
            lua::push(state(), "0.0");                // 5
            lua::push(state(), "Rainbow");            // 6
            lua_newtable(state());                    // 7
            lua_pushlightuserdata(state(), nullptr);  // 8
        }
    };
}

TEST_F(LuaHelperTest, IsBoolean)
{
    ASSERT_FALSE(lua::isboolean(state(), 1));
    ASSERT_TRUE(lua::isboolean(state(), 2));
    ASSERT_FALSE(lua::isboolean(state(), 3));
    ASSERT_FALSE(lua::isboolean(state(), 4));
    ASSERT_FALSE(lua::isboolean(state(), 5));
    ASSERT_FALSE(lua::isboolean(state(), 6));
    ASSERT_FALSE(lua::isboolean(state(), 7));
    ASSERT_FALSE(lua::isboolean(state(), 8));
}

TEST_F(LuaHelperTest, IsNumber)
{
    ASSERT_FALSE(lua::isnumber(state(), 1));
    ASSERT_FALSE(lua::isnumber(state(), 2));
    ASSERT_TRUE(lua::isnumber(state(), 3));
    ASSERT_TRUE(lua::isnumber(state(), 4));
    ASSERT_TRUE(lua::isnumber(state(), 5));
    ASSERT_FALSE(lua::isnumber(state(), 6));
    ASSERT_FALSE(lua::isnumber(state(), 7));
    ASSERT_FALSE(lua::isnumber(state(), 8));
}

TEST_F(LuaHelperTest, IsString)
{
    ASSERT_FALSE(lua::isstring(state(), 1));
    ASSERT_FALSE(lua::isstring(state(), 2));
    ASSERT_TRUE(lua::isstring(state(), 3));
    ASSERT_TRUE(lua::isstring(state(), 4));
    ASSERT_TRUE(lua::isstring(state(), 5));
    ASSERT_TRUE(lua::isstring(state(), 6));
    ASSERT_FALSE(lua::isstring(state(), 7));
    ASSERT_FALSE(lua::isstring(state(), 8));
}

TEST_F(LuaHelperTest, IsTable)
{
    ASSERT_FALSE(lua::istable(state(), 1));
    ASSERT_FALSE(lua::istable(state(), 2));
    ASSERT_FALSE(lua::istable(state(), 3));
    ASSERT_FALSE(lua::istable(state(), 4));
    ASSERT_FALSE(lua::istable(state(), 5));
    ASSERT_FALSE(lua::istable(state(), 6));
    ASSERT_TRUE(lua::istable(state(), 7));
    ASSERT_FALSE(lua::istable(state(), 8));
}

TEST_F(LuaHelperTest, IsUserData)
{
    ASSERT_FALSE(lua::isuserdata(state(), 1));
    ASSERT_FALSE(lua::isuserdata(state(), 2));
    ASSERT_FALSE(lua::isuserdata(state(), 3));
    ASSERT_FALSE(lua::isuserdata(state(), 4));
    ASSERT_FALSE(lua::isuserdata(state(), 5));
    ASSERT_FALSE(lua::isuserdata(state(), 6));
    ASSERT_FALSE(lua::isuserdata(state(), 7));
    ASSERT_TRUE(lua::isuserdata(state(), 8));
}

TEST_F(LuaHelperTest, OptionallyReturnsDefaultValue)
{
    constexpr int kDefault = -1;
    constexpr float kDefaultF = -1;

    ASSERT_EQ(kDefault, lua::optinteger(state(), 1, kDefault));
    ASSERT_EQ(42, lua::optinteger(state(), 3, kDefault));
    ASSERT_EQ(3, lua::optinteger(state(), 4, kDefault));
    ASSERT_EQ(0, lua::optinteger(state(), 5, kDefault));
    ASSERT_EQ(kDefault, lua::optinteger(state(), 9000, kDefault));

    ASSERT_EQ(kDefaultF, lua::optnumber(state(), 1, kDefaultF));
    ASSERT_EQ(42.0f, lua::optnumber(state(), 3, kDefaultF));
    ASSERT_EQ(3.14f, lua::optnumber(state(), 4, kDefaultF));
    ASSERT_EQ(0.0f, lua::optnumber(state(), 5, kDefaultF));
    ASSERT_EQ(kDefaultF, lua::optnumber(state(), 9000, kDefaultF));
}

TEST_F(LuaHelperTest, ToBoolean)
{
    ASSERT_FALSE(lua::toboolean(state(), 1));
    ASSERT_TRUE(lua::toboolean(state(), 2));
    ASSERT_TRUE(lua::toboolean(state(), 3));
    ASSERT_TRUE(lua::toboolean(state(), 4));
    ASSERT_TRUE(lua::toboolean(state(), 5));
    ASSERT_TRUE(lua::toboolean(state(), 6));
    ASSERT_TRUE(lua::toboolean(state(), 7));
    ASSERT_TRUE(lua::toboolean(state(), 8));
    ASSERT_FALSE(lua::toboolean(state(), 9000));
}

TEST_F(LuaHelperTest, ToInteger)
{
    ASSERT_EQ(42, lua::tointeger(state(), 3));
    ASSERT_EQ(3, lua::tointeger(state(), 4));
    ASSERT_EQ(0, lua::tointeger(state(), 5));
}

TEST_F(LuaHelperTest, ToNumber)
{
    ASSERT_EQ(42.0f, lua::tonumber(state(), 3));
    ASSERT_EQ(3.14f, lua::tonumber(state(), 4));
    ASSERT_EQ(0.0f, lua::tonumber(state(), 5));
}
