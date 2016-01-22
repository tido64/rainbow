// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Random.h"

#include "Common/Random.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

NS_RAINBOW_LUA_MODULE_BEGIN(random)
{
    namespace
    {
        int random(lua_State* L)
        {
            lua_Number r;
            switch (lua_gettop(L))
            {
                case 0:  // rainbow.random()
                    r = rainbow::random();
                    break;
                case 1:  // rainbow.random(max)
                    rainbow::lua::Argument<lua_Number>::is_required(L, 1);
                    r = rainbow::random(lua_tonumber(L, 1));
                    break;
                default:  // rainbow.random(min, max)
                    rainbow::lua::Argument<lua_Number>::is_required(L, 1);
                    rainbow::lua::Argument<lua_Number>::is_required(L, 2);
                    r = rainbow::random(lua_tonumber(L, 1), lua_tonumber(L, 2));
                    break;
            }
            lua_pushnumber(L, r);
            return 1;
        }

        int seed(lua_State* L)
        {
            // rainbow.seed(seed = 0)
            rainbow::lua::Argument<lua_Number>::is_optional(L, 1);

            rainbow::random.seed(rainbow::lua::optinteger(L, 1, 0));
            return 0;
        }
    }

    void init(lua_State* L)
    {
        luaR_rawsetcfunction(L, "random", &random);
        luaR_rawsetcfunction(L, "seed", &seed);
    }
} NS_RAINBOW_LUA_MODULE_END(random)
