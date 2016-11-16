// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaHelper.h"

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"
#include "Lua/LuaDebugging.h"
#include "Lua/LuaSyntax.h"

using rainbow::lua::WeakRef;
using rainbow::string_view;

namespace
{
    constexpr char kLuaErrorErrorHandling[] = "error handling";
    constexpr char kLuaErrorGeneral[] = "general";
    constexpr char kLuaErrorMemory[] = "memory allocation";
    constexpr char kLuaErrorRuntime[] = "runtime";
    constexpr char kLuaErrorSyntax[] = "syntax";
    constexpr char kLuaErrorType[] = "Object is not of type '%s'";

    auto load_module(lua_State* L, const char* module, const char* suffix)
        -> int
    {
        const auto filename = std::string{module} + suffix;
        const auto asset = rainbow::filesystem::relative(filename.c_str());

#ifndef RAINBOW_OS_ANDROID
        std::error_code error;
        if (!rainbow::filesystem::is_regular_file(asset, error))
            return 0;
#endif  // RAINBOW_OS_ANDROID

        File file = File::open(asset);
        if (!file)
            return 0;

        const int result =
            rainbow::lua::load(L, Data(std::move(file)), module, false);
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
    int WeakRef::RegistryIndex = LUA_NOREF;

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

        const char* desc = kLuaErrorGeneral;
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
        const char* module = lua_tostring(L, -1);
        const int result = load_module(L, module, ".lua");
        return result == 0 ? load_module(L, module, "/init.lua") : result;
    }

    auto load(lua_State* L, const Data& chunk, const char* name, bool exec)
        -> int
    {
        int e = luaL_loadbuffer(L, chunk, chunk.size(), name);
        if (e == LUA_OK && exec)
            e = lua_pcall(L, 0, LUA_MULTRET, 0);
        if (e != LUA_OK)
        {
            error(L, e);
            return 0;
        }
        return 1;
    }

    auto newstate() -> std::unique_ptr<lua_State, decltype(&lua_close)>
    {
        lua_State* L = luaL_newstate();
        return {L, &lua_close};
    }

    template <>
    void push<bool>(lua_State* L, bool value)
    {
        lua_pushboolean(L, static_cast<int>(value));
    }

    template <>
    void push<const char*>(lua_State* L, const char* value)
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

    void pushpointer(lua_State* L, void* ptr, const char* name)
    {
        lua_createtable(L, 1, 1);
        lua_pushlightuserdata(L, ptr);
        lua_rawseti(L, -2, 0);
        luaR_rawsetstring(L, "__type", name);
    }

    auto reload(lua_State* L, const Data& chunk, const string_view& name) -> int
    {
        lua_getglobal(L, "package");
        lua_pushliteral(L, "loaded");
        lua_rawget(L, -2);

        R_ASSERT(lua_istable(L, -1), "Missing control table 'package.loaded'");

        lua_pushlstring(L, name.data(), name.length());
        lua_pushnil(L);
        lua_rawset(L, -3);
        lua_pop(L, 2);
        return load(L, chunk, name.data(), true);
    }

    void replacetable(lua_State* L, int n)
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

    void sethook(lua_State* L, int mask)
    {
        if (g_level >= 0)
            return;

        int depth = -1;
        lua_Debug ar;
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

    auto topointer(lua_State* L, const char* name) -> void*
    {
        LUA_ASSERT(L, !lua_isnil(L, -1), "Unexpected nil value");
        LUA_ASSERT(L, lua_istable(L, -1), kLuaErrorType, name);
        lua_pushliteral(L, "__type");
        lua_rawget(L, -2);
        const char* type = lua_tostring(L, -1);
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
} NS_RAINBOW_LUA_END
