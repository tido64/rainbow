// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaScript.h"

#include <lua.hpp>

#include "Common/Data.h"
#include "Lua/lua_Input.h"
#include "Lua/lua_Platform.h"

using rainbow::GameBase;
using rainbow::KeyStroke;

LuaScript::~LuaScript() { lua_.close(); }

void LuaScript::init_impl(const Vec2i& screen)
{
    if (lua_.init(this, &scenegraph()) != LUA_OK)
    {
        terminate("Failed to initialise Lua");
        return;
    }

    rainbow::lua::platform::update(lua_, screen);

    const Data& main = Data::load_asset("main.lua");
    R_ASSERT(main, "Failed to load 'main.lua'");
    if (lua_.start(main) != LUA_OK || lua_.update(0) != LUA_OK)
    {
        terminate("Failed to start 'main.lua'");
        return;
    }

    input().subscribe(this);
}

void LuaScript::update_impl(uint64_t dt)
{
    if (lua_.update(dt))
        terminate();
    rainbow::lua::input::clear(lua_);
}

void LuaScript::on_memory_warning_impl()
{
    lua_gc(lua_, LUA_GCCOLLECT, 0);
}

bool LuaScript::on_key_down_impl(const KeyStroke& k)
{
    rainbow::lua::input::on_key_down(lua_, k);
    return true;
}

bool LuaScript::on_key_up_impl(const KeyStroke& k)
{
    rainbow::lua::input::on_key_up(lua_, k);
    return true;
}

bool LuaScript::on_pointer_began_impl(const ArrayView<Pointer>& pointers)
{
    rainbow::lua::input::on_pointer_began(lua_, pointers);
    return true;
}

bool LuaScript::on_pointer_canceled_impl()
{
    rainbow::lua::input::on_pointer_canceled(lua_);
    return true;
}

bool LuaScript::on_pointer_ended_impl(const ArrayView<Pointer>& pointers)
{
    rainbow::lua::input::on_pointer_ended(lua_, pointers);
    return true;
}

bool LuaScript::on_pointer_moved_impl(const ArrayView<Pointer>& pointers)
{
    rainbow::lua::input::on_pointer_moved(lua_, pointers);
    return true;
}

auto GameBase::create(rainbow::Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<LuaScript>(director);
}
