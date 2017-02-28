// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaMachine.h"

#include "Common/Chrono.h"
#include "Common/Data.h"
#include "Lua/LuaModules.h"
#include "Lua/LuaScript.h"
#include "Resources/Rainbow.lua.h"

using rainbow::Chrono;
using rainbow::Data;
using rainbow::LuaMachine;
using rainbow::LuaScript;
using rainbow::graphics::RenderQueue;

namespace
{
    constexpr char kLuaRainbowInstance[] = "__rainbow_instance";
    constexpr int kNumErrFuncs = IF_DEBUG_ELSE(1, 0);
    constexpr char kRainbow[] = "rainbow";

    auto breakpoint(lua_State* L)
    {
        IF_DEBUG_ELSE(rainbow::lua::sethook(L), NOT_USED(L));
        return 0;
    }

    auto createtable(lua_State* L)
    {
        using rainbow::lua::optinteger;
        lua_createtable(L, optinteger(L, 1, 0), optinteger(L, 2, 0));
        return 1;
    }

    auto exit(lua_State* L)
    {
        rainbow::lua::checkargs<rainbow::lua::nil_or<char*>>(L);

        lua_getglobal(L, kLuaRainbowInstance);
        if (lua_islightuserdata(L, -1))
        {
            auto instance = static_cast<LuaScript*>(lua_touserdata(L, -1));
            if (lua_gettop(L) > 1)
                instance->terminate(lua_tostring(L, 1));
            else
                instance->terminate();
            lua_pushnil(L);
            lua_setglobal(L, kLuaRainbowInstance);
        }
        lua_pop(L, 1);
        return 0;
    }

    // Copied from 'linit.c'.
    void luaR_openlibs(lua_State* L)
    {
        const luaL_Reg loadedlibs[]{
            {"_G", luaopen_base},
            {LUA_LOADLIBNAME, luaopen_package},
            {LUA_COLIBNAME, luaopen_coroutine},
            {LUA_TABLIBNAME, luaopen_table},
            {LUA_STRLIBNAME, luaopen_string},
            {LUA_BITLIBNAME, luaopen_bit32},
            {LUA_MATHLIBNAME, luaopen_math},
            {LUA_DBLIBNAME, luaopen_debug}};

        // Call open functions from 'loadedlibs' and set results to global table
        for (const auto& lib : loadedlibs)
        {
            luaL_requiref(L, lib.name, lib.func, 1);
            lua_pop(L, 1);
        }
    }

    auto time_since_epoch(lua_State* L)
    {
        lua_pushinteger(L, Chrono::time_since_epoch().count());
        return 1;
    }
}

LuaMachine::LuaMachine(rainbow::Passkey<LuaScript>)
    : state_(lua::newstate().release()), internal_(0)
#ifndef NDEBUG
    , traceback_(0)
#endif
{
}

LuaMachine::~LuaMachine()
{
    R_ASSERT(
        !state_, "LuaMachine must be terminated before the graphics context");
}

void LuaMachine::close()
{
    if (state_ == nullptr)
        return;

    lua::WeakRef::RegistryIndex = LUA_NOREF;
    lua_pushnil(state_);
    lua_setglobal(state_, kLuaRainbowInstance);

    // Remove "rainbow.renderqueue".
    lua_getglobal(state_, kRainbow);
    lua_pushstring(state_, lua::RenderQueue::class_name);
    lua_pushnil(state_);
    lua_rawset(state_, -3);
    lua_pop(state_, 1);

    lua_close(state_);
    state_ = nullptr;
}

auto LuaMachine::init(LuaScript& instance, RenderQueue& queue) -> int
{
    luaR_openlibs(state_);

    // Initialize "rainbow" namespace.
    lua_createtable(state_, 0, 16);
    lua::init(state_);

    // Set "rainbow.breakpoint".
    luaR_rawsetcfunction(state_, "breakpoint", breakpoint);

    // Set "rainbow.exit".
    lua_pushlightuserdata(state_, &instance);
    lua_setglobal(state_, kLuaRainbowInstance);
    luaR_rawsetcfunction(state_, "exit", &exit);

    // Set "rainbow.time_since_epoch".
    luaR_rawsetcfunction(state_, "time_since_epoch", time_since_epoch);

    // Bind C++ objects.
    lua::bind(state_);

    // Initialize "rainbow.renderqueue".
    lua_pushstring(state_, lua::RenderQueue::class_name);
    lua::alloc<lua::RenderQueue>(state_, queue);
    lua_rawset(state_, -3);

    lua_setglobal(state_, kRainbow);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

    // Set up custom loader.
    lua_getglobal(state_, "package");

    R_ASSERT(!lua_isnil(state_, -1), "package table does not exist");

    lua_pushliteral(state_, "searchers");
    lua_rawget(state_, -2);

    R_ASSERT(!lua_isnil(state_, -1), "package.searchers table does not exist");

    // Make space in the top slot for our loader.
    for (int i = static_cast<int>(lua_rawlen(state_, -1)); i > 0; --i)
    {
        lua_rawgeti(state_, -1, i);
        lua_rawseti(state_, -2, i + 1);
    }
    lua_pushcfunction(state_, lua::load);
    lua_rawseti(state_, -2, 1);
    lua_pop(state_, 2);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

    // Load our internal script.
    if (lua::load(state_, Data::from_literal(Rainbow_lua), kRainbow) == 0)
        return luaL_error(state_, "Failed to load internal Lua script");

    lua_getglobal(state_, "__update");

    R_ASSERT(lua_isfunction(state_, -1), "Failed to get internal Lua script");

    internal_ = luaL_ref(state_, LUA_REGISTRYINDEX);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

#ifndef NDEBUG
    lua_getglobal(state_, "debug");
    lua_pushliteral(state_, "traceback");
    lua_rawget(state_, -2);
    traceback_ = luaL_ref(state_, LUA_REGISTRYINDEX);
    lua_pop(state_, 1);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");
#endif

    // Create table for lua::WeakRef
    lua_createtable(state_, 1, 1);
    lua_createtable(state_, 0, 1);
    lua_pushliteral(state_, "__mode");
    lua_pushliteral(state_, "v");
    lua_rawset(state_, -3);
    lua_setmetatable(state_, -2);
    lua_pushinteger(state_, 0);
    lua_rawseti(state_, -2, 0);
    lua::WeakRef::RegistryIndex = luaL_ref(state_, LUA_REGISTRYINDEX);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

    // Map 'lua_createtable' to 'table.create'
    lua_getglobal(state_, "table");
    lua_pushliteral(state_, "create");
    lua_pushcfunction(state_, createtable);
    lua_rawset(state_, -3);
    lua_pop(state_, 1);

    R_ASSERT(lua_gettop(state_) == 0, "Stack not empty");

    return LUA_OK;
}

auto LuaMachine::start(const Data& main) -> int
{
    if (lua::load(state_, main, "main") == 0)
        return luaL_error(state_, "Failed to load main script");

    IF_DEBUG(lua_rawgeti(state_, LUA_REGISTRYINDEX, traceback_));

    lua_getglobal(state_, "init");
    return lua::call(
        state_, 0, 0, kNumErrFuncs, "Failed to initialise main script");
}

auto LuaMachine::update(uint64_t t) -> int
{
    IF_DEBUG(lua_rawgeti(state_, LUA_REGISTRYINDEX, traceback_));

    lua_rawgeti(state_, LUA_REGISTRYINDEX, internal_);
    lua_pushinteger(state_, t);
    return lua::call(state_, 1, 0, kNumErrFuncs, "Failed to call 'update'");
}
