// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaScript.h"

#include <lua.hpp>

#include "Common/Data.h"
#include "Lua/lua_Input.h"
#include "Lua/lua_Platform.h"

LuaScript::~LuaScript()
{
	lua_.close();
}

void LuaScript::init(const Vec2i &screen)
{
	if (lua_.init(&scenegraph()) != LUA_OK)
	{
		terminate("Failed to initialise Lua");
		return;
	}
	const Data &main = Data::load_asset("main.lua");
	R_ASSERT(main, "Failed to load 'main.lua'");
	rainbow::lua::platform::update(lua_, screen);
	if (lua_.start(main) != LUA_OK || lua_.update(0) != LUA_OK)
	{
		terminate("Failed to start 'main.lua'");
		return;
	}
	input().subscribe(this);
}

void LuaScript::update(const unsigned long dt)
{
	if (lua_.update(dt))
		terminate();
	rainbow::lua::input::clear(lua_);
}

void LuaScript::on_memory_warning()
{
	lua_gc(lua_, LUA_GCCOLLECT, 0);
}

#ifdef RAINBOW_BUTTONS
bool LuaScript::on_key_down_impl(const Key &k)
{
	rainbow::lua::input::on_key_down(lua_, k);
	return true;
}

bool LuaScript::on_key_up_impl(const Key &k)
{
	rainbow::lua::input::on_key_up(lua_, k);
	return true;
}
#endif  // RAINBOW_BUTTONS

bool LuaScript::on_touch_began_impl(const Touch *const touches,
                                    const size_t count)
{
	rainbow::lua::input::on_touch_began(lua_, touches, count);
	return true;
}

bool LuaScript::on_touch_canceled_impl()
{
	rainbow::lua::input::on_touch_canceled(lua_);
	return true;
}

bool LuaScript::on_touch_ended_impl(const Touch *const touches,
                                    const size_t count)
{
	rainbow::lua::input::on_touch_ended(lua_, touches, count);
	return true;
}

bool LuaScript::on_touch_moved_impl(const Touch *const touches,
                                    const size_t count)
{
	rainbow::lua::input::on_touch_moved(lua_, touches, count);
	return true;
}

GameBase* GameBase::create(rainbow::Director &director)
{
	return new LuaScript(director);
}
