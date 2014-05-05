// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SpriteBatch.h"

#include "FileSystem/Path.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_Texture.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char SpriteBatch::Bind::class_name[] = "spritebatch";

	template<>
	const bool SpriteBatch::Bind::is_constructible = true;

	template<>
	const luaL_Reg SpriteBatch::Bind::functions[] = {
		{ "add",            &SpriteBatch::add },
		{ "create_sprite",  &SpriteBatch::create_sprite },
		{ "set_normal",     &SpriteBatch::set_normal },
		{ "set_texture",    &SpriteBatch::set_texture },
		{ nullptr, nullptr }
	};

	SpriteBatch::SpriteBatch(lua_State *L) : batch(luaR_optinteger(L, 1, 4)) { }

	int SpriteBatch::add(lua_State *L)
	{
		R_DEBUG("[Rainbow] <spritebatch>:add() is deprecated, use "
		        "<spritebatch>:create_sprite() and <sprite>:set_texture() "
		        "instead.\n");

		LUA_ASSERT(lua_isnumber(L, 2) &&
		           lua_isnumber(L, 3) &&
		           lua_isnumber(L, 4) &&
		           lua_isnumber(L, 5),
		           "<spritebatch>:add(x, y, width, height)");

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		const int x = lua_tointeger(L, 2);
		const int y = lua_tointeger(L, 3);
		const int w = lua_tointeger(L, 4);
		const int h = lua_tointeger(L, 5);

		lua_settop(L, 0);
		lua_pushlightuserdata(L, self->get());
		lua_pushinteger(L, self->batch.add(x, y, w, h));
		return alloc<Sprite>(L);
	}

	int SpriteBatch::create_sprite(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2) && lua_isnumber(L, 3),
		           "<spritebatch>:create_sprite(width, height)");

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		const int w = lua_tointeger(L, 2);
		const int h = lua_tointeger(L, 3);

		lua_settop(L, 0);
		lua_pushlightuserdata(L, self->get());
		lua_pushinteger(L, self->batch.create_sprite(w, h));
		return alloc<Sprite>(L);
	}

	int SpriteBatch::set_normal(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "<spritebatch>:set_normal(<texture>)");

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		self->batch.set_normal(touserdata<Texture>(L, 2)->get());
		return 0;
	}

	int SpriteBatch::set_texture(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2),
		           "<spritebatch>:set_texture(<texture>)");

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		self->batch.set_texture(touserdata<Texture>(L, 2)->get());
		return 0;
	}
} NS_RAINBOW_LUA_END
