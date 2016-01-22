// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Helper.h"

namespace b2
{
    b2Vec2 Vec2(lua_State* L, int m, int n)
    {
        return b2Vec2(lua_tonumber(L, m), lua_tonumber(L, n));
    }

    void getfield(lua_State* L, const char* field, size_t length)
    {
        lua_pushlstring(L, field, length);
        lua_gettable(L, -2);
        LUA_ASSERT(
            L, !lua_isnil(L, -1), "Missing field '%s' in definition", field);
    }

    template <>
    bool gettable<bool>(lua_State* L, const char* field, size_t length)
    {
        getfield(L, field, length);
        const bool b = lua_toboolean(L, -1);
        lua_pop(L, 1);
        return b;
    }

    template <>
    lua_Integer gettable<lua_Integer>(lua_State* L,
                                      const char* field,
                                      size_t length)
    {
        getfield(L, field, length);
        const lua_Integer i = rainbow::lua::tointeger(L, -1);
        lua_pop(L, 1);
        return i;
    }

    template <>
    lua_Number gettable<lua_Number>(lua_State* L,
                                    const char* field,
                                    size_t length)
    {
        getfield(L, field, length);
        const lua_Number n = rainbow::lua::tonumber(L, -1);
        lua_pop(L, 1);
        return n;
    }

    template <>
    b2Vec2 gettable<b2Vec2>(lua_State* L, const char* field, size_t length)
    {
        getfield(L, field, length);
        const b2Vec2 v(luaR_getnumber(L, "x"), luaR_getnumber(L, "y"));
        lua_pop(L, 1);
        return v;
    }

    void rawsetnil(lua_State* L, const char* field, size_t length)
    {
        lua_pushlstring(L, field, length);
        lua_pushnil(L);
        lua_rawset(L, -3);
    }

    int rawsetvec2(lua_State* L, const char* field, size_t length)
    {
        lua_pushlstring(L, field, length);
        lua_createtable(L, 0, 2);
        luaR_rawsetnumber(L, "x", 0.0f);
        luaR_rawsetnumber(L, "y", 0.0f);
        lua_rawset(L, -3);
        return 1;
    }
}
