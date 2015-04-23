// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef NDEBUG

#include "Lua/LuaSyntax.h"

#include <lua.hpp>

class Drawable;

namespace ConFuoco
{
	class Channel;
	class Sound;
}

namespace rainbow { class SceneNode; }

namespace b2
{
	namespace lua
	{
		class Body;
		class Fixture;
	}
}

namespace FMOD
{
	class Sound;

	namespace Studio
	{
		class Bank;
	}
}

namespace
{
	bool is_boolean(lua_State *L, const int n)
	{
		return lua_type(L, n) == LUA_TBOOLEAN;
	}

	bool is_table(lua_State *L, const int n)
	{
		return lua_type(L, n) == LUA_TTABLE;
	}

	bool is_userdata(lua_State *L, const int n)
	{
		return lua_isuserdata(L, n) || is_table(L, n);
	}

	int type_error(lua_State *L, const int n, const char *const type) {
		const char *msg = lua_pushfstring(
		    L, "%s expected, got %s", type, luaL_typename(L, n));
		return luaL_argerror(L, n, msg);
	}

	template<typename F>
	void optional(lua_State *L, const int n, F &&is_type, const char *const type)
	{
		if (!lua_isnoneornil(L, n) && !is_type(L, n))
			type_error(L, n, type);
	}

	template<typename F>
	void require(lua_State *L, const int n, F &&is_type, const char *const type)
	{
		if (!is_type(L, n))
			type_error(L, n, type);
	}
}

NS_RAINBOW_LUA_BEGIN
{
	/* bool */

	template<>
	void Argument<bool>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, is_boolean, "nil or boolean");
	}

	template<>
	void Argument<bool>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_boolean, "boolean");
	}

	/* char* */

	template<>
	void Argument<char*>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, lua_isstring, "nil or string");
	}

	template<>
	void Argument<char*>::is_required(lua_State *L, const int n)
	{
		require(L, n, lua_isstring, "string");
	}

	/* void */

	template<>
	void Argument<void*>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, is_table, "table");
	}

	template<>
	void Argument<void*>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_table, "table");
	}

	/* Drawable */

	template<>
	void Argument<Drawable>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "drawable");
	}

	/* lua_Number */

	template<>
	void Argument<lua_Number>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, lua_isnumber, "nil or number");
	}

	template<>
	void Argument<lua_Number>::is_required(lua_State *L, const int n)
	{
		require(L, n, lua_isnumber, "number");
	}

	/* rainbow::lua::Animation */

	class Animation;

	template<>
	void Argument<Animation>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "animation");
	}

	/* rainbow::lua::Font */

	class Font;

	template<>
	void Argument<Font>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "font");
	}

	/* rainbow::lua::Label */

	class Label;

	template<>
	void Argument<Label>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "label");
	}

	/* rainbow::lua::Shader */

	class Shader;

	template<>
	void Argument<Shader>::is_required(lua_State *L, const int n)
	{
		if (!((lua_isnumber(L, n) && lua_tointeger(L, n) == 0) ||
		        lua_isuserdata(L, n))) {
			luaL_argerror(L, n, "shader");
		}
	}

	/* rainbow::lua::Sprite */

	class Sprite;

	template<>
	void Argument<Sprite>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, is_userdata, "nil or sprite");
	}

	template<>
	void Argument<Sprite>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "sprite");
	}

	/* rainbow::lua::SpriteBatch */

	class SpriteBatch;

	template<>
	void Argument<SpriteBatch>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "sprite batch");
	}

	/* rainbow::lua::Texture */

	class Texture;

	template<>
	void Argument<Texture>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "texture");
	}

	/* rainbow::SceneNode */

	template<>
	void Argument<SceneNode>::is_optional(lua_State *L, const int n)
	{
		optional(L, n, lua_isuserdata, "nil or node");
	}

	template<>
	void Argument<SceneNode>::is_required(lua_State *L, const int n)
	{
		require(L, n, lua_isuserdata, "node");
	}

	/* Box2D */

	template<>
	void Argument<b2::lua::Body>::is_required(lua_State *L, const int n)
	{
		luaL_checkudata(L, n, "b2Body");
	}

	template<>
	void Argument<b2::lua::Fixture>::is_required(lua_State *L, const int n)
	{
		luaL_checkudata(L, n, "b2Fixture");
	}

	/* ConFuoco */

	template<>
	void Argument<ConFuoco::Channel>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_table, "channel");
	}

	template<>
	void Argument<ConFuoco::Sound>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_table, "sound");
	}

	/* FMOD Studio */

	template<>
	void Argument<FMOD::Sound>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_userdata, "FMOD::Sound");
	}

	template<>
	void Argument<FMOD::Studio::Bank>::is_required(lua_State *L, const int n)
	{
		require(L, n, is_table, "FMOD::Studio::Bank");
	}
} NS_RAINBOW_LUA_END

#endif
