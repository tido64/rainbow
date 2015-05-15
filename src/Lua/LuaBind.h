// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_LUABIND_H_
#define LUA_LUABIND_H_

#include "Common/Vec2.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

NS_RAINBOW_LUA_BEGIN
{
	template<typename T>
	class Bind
	{
	public:
		static const char class_name[];
		static const bool is_constructible;
		static const luaL_Reg functions[];

	protected:
		static T* self(lua_State *L)
		{
			return touserdata<T>(L, 1);
		}

		template<typename F, typename... Args>
		static int get1b(lua_State *L, F&& get, Args&&... args)
		{
			T *self = Bind::self(L);
			if (!self)
				return 0;

			lua_pushboolean(L, get(self->get(), std::forward<Args>(args)...));
			return 1;
		}

#ifdef USE_PHYSICS
		template<typename F>
		static int set1b(lua_State *L, F&& set)
		{
			Argument<bool>::is_required(L, 2);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), lua_toboolean(L, 2));
			return 0;
		}
#endif  // USE_PHYSICS

		template<typename F>
		static int get1f(lua_State *L, F&& get)
		{
			T *self = Bind::self(L);
			if (!self)
				return 0;

			lua_pushnumber(L, get(self->get()));
			return 1;
		}

		template<typename F>
		static int set1f(lua_State *L, F&& set)
		{
			Argument<lua_Number>::is_required(L, 2);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), lua_tonumber(L, 2));
			return 0;
		}

		template<typename F, typename... Args>
		static int get1fv(lua_State *L, F&& get, Args&&... args)
		{
			T *self = Bind::self(L);
			if (!self)
				return 0;

			const auto &v = get(self->get(), std::forward<Args>(args)...);
			lua_pushnumber(L, v.x);
			lua_pushnumber(L, v.y);
			return 2;
		}

		template<typename F>
		static int set1fv(lua_State *L, F&& set)
		{
			Argument<lua_Number>::is_required(L, 2);
			Argument<lua_Number>::is_required(L, 3);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), Vec2f(lua_tonumber(L, 2), lua_tonumber(L, 3)));
			return 0;
		}

		template<typename F>
		static int set2f(lua_State *L, F&& set)
		{
			Argument<lua_Number>::is_required(L, 2);
			Argument<lua_Number>::is_required(L, 3);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), lua_tonumber(L, 2), lua_tonumber(L, 3));
			return 0;
		}

		template<typename F>
		static int get1i(lua_State *L, F&& get)
		{
			T *self = Bind::self(L);
			if (!self)
				return 0;

			lua_pushinteger(L, get(self->get()));
			return 1;
		}

		template<typename F>
		static int set1i(lua_State *L, F&& set)
		{
			Argument<lua_Number>::is_required(L, 2);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), lua_tointeger(L, 2));
			return 0;
		}

		template<typename U, typename F>
		static int set1ud(lua_State *L, F&& set)
		{
			Argument<U>::is_required(L, 2);

			T *self = Bind::self(L);
			if (!self)
				return 0;

			set(self->get(), touserdata<U>(L, 2)->get());
			return 0;
		}

#ifdef USE_PHYSICS
		template<typename U, typename F>
		static int get1ud(lua_State *L, F&& get)
		{
			T *self = Bind::self(L);
			if (!self)
				return 0;

			void *p = get(self->get());
			if (!p)
				return 0;

			lua_pushlightuserdata(L, p);
			return rainbow::lua::alloc<U>(L);
		}
#endif  // USE_PHYSICS

		~Bind() = default;
	};
} NS_RAINBOW_LUA_END

#endif
