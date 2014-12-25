// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SpriteBatch.h"

#include "FileSystem/Path.h"
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

	SpriteBatch::SpriteBatch(lua_State *L) : batch_(optinteger(L, 1, 4)) {}

	int SpriteBatch::add(lua_State *L)
	{
		LOGW("<spritebatch>:add() is deprecated, use "
		     "<spritebatch>:create_sprite() and <sprite>:set_texture() "
		     "instead.");

		// <spritebatch>:add(x, y, width, height)
		Argument<lua_Number>::is_required(L, 2);
		Argument<lua_Number>::is_required(L, 3);
		Argument<lua_Number>::is_required(L, 4);
		Argument<lua_Number>::is_required(L, 5);

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		const int x = lua_tointeger(L, 2);
		const int y = lua_tointeger(L, 3);
		const int w = lua_tointeger(L, 4);
		const int h = lua_tointeger(L, 5);

		lua_settop(L, 0);
		lua_pushlightuserdata(L, self->get());
		lua_pushinteger(L, self->batch_.add(x, y, w, h));
		return alloc<Sprite>(L);
	}

	int SpriteBatch::create_sprite(lua_State *L)
	{
		// <spritebatch>:create_sprite(width, height)
		Argument<lua_Number>::is_required(L, 2);
		Argument<lua_Number>::is_required(L, 3);

		SpriteBatch *self = Bind::self(L);
		if (!self)
			return 0;

		const int w = lua_tointeger(L, 2);
		const int h = lua_tointeger(L, 3);

		lua_settop(L, 0);
		lua_pushlightuserdata(L, self->get());
		lua_pushinteger(L, self->batch_.create_sprite(w, h));
		return alloc<Sprite>(L);
	}

	int SpriteBatch::set_normal(lua_State *L)
	{
		// <spritebatch>:set_normal(<texture>)
		return set1ud<Texture>(
		    L,
		    [](::SpriteBatch *batch, SharedPtr<TextureAtlas> atlas) {
		      batch->set_normal(atlas);
		    });
	}

	int SpriteBatch::set_texture(lua_State *L)
	{
		// <spritebatch>:set_texture(<texture>)
		return set1ud<Texture>(
		    L,
		    [](::SpriteBatch *batch, SharedPtr<TextureAtlas> atlas) {
		      batch->set_texture(atlas);
		    });
	}
} NS_RAINBOW_LUA_END
