// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/Animation.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Sprite.h"
#include "Lua/lua_Animation.h"

NS_RAINBOW_LUA_BEGIN
{
	template<>
	const char Animation::Bind::class_name[] = "animation";

	template<>
	const bool Animation::Bind::is_constructible = true;

	template<>
	const luaL_Reg Animation::Bind::functions[] = {
		{ "is_stopped",  &Animation::is_stopped },
		{ "set_delay",   &Animation::set_delay },
		{ "set_fps",     &Animation::set_fps },
		{ "set_frames",  &Animation::set_frames },
		{ "set_sprite",  &Animation::set_sprite },
		{ "play",        &Animation::play },
		{ "stop",        &Animation::stop },
		{ nullptr, nullptr }
	};

	Animation::Animation(lua_State *L) : animation(nullptr)
	{
		LUA_ASSERT((luaR_isuserdata(L, 1) || lua_isnil(L, 1)) &&
		           lua_istable(L, 2) &&
		           lua_isnumber(L, 3) &&
		           (lua_isnumber(L, 4) || lua_isnone(L, 4)),
		           "rainbow.animation(sprite, frames{}, fps, loop_delay = 0)");

		replacetable(L, 1);
		::Sprite *sprite =
		    lua_isuserdata(L, 1) ? touserdata<Sprite>(L, 1)->get() : nullptr;

		// Count number of frames
		unsigned int count = 0;
		lua_pushnil(L);
		while (lua_next(L, 2))
		{
			++count;
			lua_pop(L, 1);
		}

		// Allocate frames array
		unsigned int *const frames = new unsigned int[count + 1];
		unsigned int *frame = frames;

		// Populate frames array
		lua_pushnil(L);
		while (lua_next(L, 2))
		{
			*frame = luaR_tointeger(L, -1);
			++frame;
			lua_pop(L, 1);
		}
		frames[count] = ::Animation::kAnimationEnd;

		const unsigned int fps = lua_tointeger(L, 3);
		const int delay = luaR_optinteger(L, 4, 0);
		this->animation = new ::Animation(sprite, frames, fps, delay);
	}

	Animation::~Animation()
	{
		delete this->animation;
	}

	int Animation::is_stopped(lua_State *L)
	{
		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		lua_pushboolean(L, self->animation->is_stopped());
		return 1;
	}

	int Animation::set_delay(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2), "<animation>:set_loop(delay_in_ms)");

		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		self->animation->set_delay(lua_tointeger(L, 2));
		return 0;
	}

	int Animation::set_fps(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 2), "<animation>:set_fps(fps)");

		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		self->animation->set_timeout(1000.0f / lua_tointeger(L, 2));
		return 0;
	}

	int Animation::set_frames(lua_State *L)
	{
		LUA_ASSERT(lua_gettop(L) > 2, "<animation>:set_frames(f1, f2, ...)");

		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		const int count = lua_gettop(L) - 1;
		unsigned int *const frames = new unsigned int[count + 1];
		for (int i = 0; i < count; ++i)
			frames[i] = luaR_tointeger(L, i + 2);
		frames[count] = 0;
		self->animation->set_frames(frames);
		return 0;
	}

	int Animation::set_sprite(lua_State *L)
	{
		LUA_ASSERT(lua_isuserdata(L, 2), "<animation>:set_sprite(sprite)");

		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		self->animation->set_sprite(touserdata<Sprite>(L, 2)->get());
		return 0;
	}

	int Animation::play(lua_State *L)
	{
		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		self->animation->start();
		return 0;
	}

	int Animation::stop(lua_State *L)
	{
		Animation *self = Bind::self(L);
		if (!self)
			return 0;

		self->animation->stop();
		self->animation->reset();
		return 0;
	}
} NS_RAINBOW_LUA_END
