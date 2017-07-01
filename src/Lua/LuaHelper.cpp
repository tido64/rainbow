// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "Lua/LuaDebugging.h"
#include "Lua/LuaSyntax.h"

using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::czstring;
using rainbow::lua::WeakRef;

namespace
{
    constexpr char kLuaErrorErrorHandling[] = "error handling";
    constexpr char kLuaErrorGeneral[] = "general";
    constexpr char kLuaErrorMemory[] = "memory allocation";
    constexpr char kLuaErrorRuntime[] = "runtime";
    constexpr char kLuaErrorSyntax[] = "syntax";
    constexpr char kLuaErrorType[] = "Object is not of type '%s'";

    auto load_module(lua_State* L, czstring module, czstring suffix) -> int
    {
        const auto filename = std::string{module} + suffix;
        const Data& chunk = File::read(filename.c_str(), FileType::Asset);
        if (!chunk)
            return 0;

        const int result = rainbow::lua::load(L, chunk, module, false);
        return result == 0 ? luaL_error(L, "Failed to load '%s'", module)
                           : result;
    }

    auto weak_ref(lua_State* L) -> int
    {
        if (L == nullptr || WeakRef::RegistryIndex < 0)
            return LUA_NOREF;

        lua_rawgeti(L, LUA_REGISTRYINDEX, WeakRef::RegistryIndex);
        lua_insert(L, -2);
        const int ref = luaL_ref(L, -2);
        lua_pop(L, 1);
        return ref;
    }

    void weak_unref(lua_State* L, const int ref)
    {
        if (ref < 0 || WeakRef::RegistryIndex < 0)
            return;

        lua_rawgeti(L, LUA_REGISTRYINDEX, WeakRef::RegistryIndex);
        luaL_unref(L, -1, ref);
        lua_pop(L, 1);
    }
}

NS_RAINBOW_LUA_BEGIN
{
    int WeakRef::RegistryIndex = LUA_NOREF;  // NOLINT

    WeakRef::WeakRef(lua_State* L) : state_(L), ref_(weak_ref(L)) {}

    WeakRef::~WeakRef() { weak_unref(state_, ref_); }

    void WeakRef::get() const
    {
        if (state_ == nullptr || ref_ < 0 || RegistryIndex < 0)
        {
            lua_pushnil(state_);
            return;
        }

        lua_rawgeti(state_, LUA_REGISTRYINDEX, RegistryIndex);
        lua_rawgeti(state_, -1, ref_);
        lua_remove(state_, -2);
    }

    void WeakRef::reset(lua_State * L)
    {
        weak_unref(state_, ref_);
        state_ = L;
        ref_ = weak_ref(state_);
    }

    void error(lua_State* L, int result)
    {
        R_ASSERT(result != LUA_OK, "No error to report");

        czstring desc = kLuaErrorGeneral;
        switch (result)
        {
            case LUA_ERRRUN:
                desc = kLuaErrorRuntime;
                break;
            case LUA_ERRSYNTAX:
                desc = kLuaErrorSyntax;
                break;
            case LUA_ERRMEM:
                desc = kLuaErrorMemory;
                break;
            case LUA_ERRERR:
                desc = kLuaErrorErrorHandling;
                break;
            default:
                break;
        }

        LOGE("Lua %s error: %s", desc, lua_tostring(L, -1));
        lua_pop(L, 1);
        dump_stack(L);
    }

    auto load(lua_State* L) -> int
    {
        czstring module = lua_tostring(L, -1);
        const int result = load_module(L, module, ".lua");
        return result == 0 ? load_module(L, module, "/init.lua") : result;
    }

    auto load(lua_State* L, const Data& chunk, czstring name, bool exec)
        -> int
    {
        int e = luaL_loadbuffer(L, chunk.as<char*>(), chunk.size(), name);
        if (e == LUA_OK && exec)
            e = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (e != LUA_OK)
        {
            error(L, e);
            return 0;
        }
        return 1;
    }

    template <>
    void push<bool>(lua_State* L, bool value)
    {
        lua_pushboolean(L, static_cast<int>(value));
    }

    template <>
    void push<czstring>(lua_State* L, czstring value)
    {
        lua_pushstring(L, value);
    }

    template <>
    void push<lua_CFunction>(lua_State* L, lua_CFunction value)
    {
        lua_pushcfunction(L, value);
    }

    template <>
    void push<lua_Integer>(lua_State* L, lua_Integer value)
    {
        lua_pushinteger(L, value);
    }

    template <>
    void push<lua_Number>(lua_State* L, lua_Number value)
    {
        lua_pushnumber(L, value);
    }

    void pushpointer(lua_State* L, void* ptr, czstring name)
    {
        lua_createtable(L, 1, 1);
        lua_pushlightuserdata(L, ptr);
        lua_rawseti(L, -2, 0);
        luaR_rawsetstring(L, "__type", name);
    }

    auto reload(lua_State* L, const Data& chunk, czstring name) -> int
    {
        lua_getglobal(L, "package");
        lua_pushliteral(L, "loaded");
        lua_rawget(L, -2);

        R_ASSERT(lua_istable(L, -1), "Missing control table 'package.loaded'");

        lua_pushstring(L, name);
        lua_pushnil(L);
        lua_rawset(L, -3);
        lua_pop(L, 2);
        return load(L, chunk, name, true);
    }

    void sethook(lua_State* L, int mask)
    {
        if (g_level >= 0)
            return;

        int depth = -1;
        lua_Debug ar{};
        while (lua_getstack(L, ++depth, &ar) != 0) {}
        --depth;
        while (lua_getstack(L, ++g_level, &ar) != 0)
        {
            lua_getinfo(L, "Sl", &ar);
            g_callstack[depth - g_level].currentline = ar.currentline;
            g_callstack[depth - g_level].nparams = ar.nparams;
            g_callstack[depth - g_level].source = ar.source;
        }

        lua_sethook(L, lua_Hook, mask, 0);
    }

    auto topointer(lua_State* L, czstring name) -> void*
    {
        LUA_ASSERT(L, !lua_isnil(L, -1), "Unexpected nil value");
        LUA_ASSERT(L, lua_istable(L, -1), kLuaErrorType, name);

        lua_pushliteral(L, "__type");
        lua_rawget(L, -2);
        czstring type = lua_tostring(L, -1);
        if (type == nullptr)
        {
            LUA_ASSERT(L, type, kLuaErrorType, name);
            lua_pop(L, 1);
            return nullptr;
        }

        LUA_ASSERT(L, strcmp(type, name) == 0, kLuaErrorType, name);

        lua_rawgeti(L, -2, 0);
        void* ptr = lua_touserdata(L, -1);
        lua_pop(L, 2);
        return ptr;

        NOT_USED(name);
        NOT_USED(kLuaErrorType);
    }

    void unwrapuserdata(lua_State* L, int n)
    {
        if (!lua_istable(L, n))
            return;

        lua_pushliteral(L, "__userdata");
        lua_rawget(L, n);
        if (!isuserdata(L, -1))
        {
            lua_pop(L, 1);
            return;
        }
        lua_replace(L, n);
    }
} NS_RAINBOW_LUA_END
