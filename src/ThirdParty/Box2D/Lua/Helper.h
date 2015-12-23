// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_HELPER_H_
#define THIRDPARTY_BOX2D_LUA_HELPER_H_

#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <Box2D/Common/b2Math.h>
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif

#include "Lua/LuaBind.h"

#define NS_B2_LUA_BEGIN namespace b2 { namespace lua
#define NS_B2_LUA_END }

#define luaR_getboolean(L, k)  gettable<bool>(L, k, strllen(k))
#define luaR_getfield(L, k)    getfield(L, k, strllen(k))
#define luaR_getinteger(L, k)  gettable<lua_Integer>(L, k, strllen(k))
#define luaR_getnumber(L, k)   gettable<lua_Number>(L, k, strllen(k))
#define luaR_getvec2(L, k)     gettable<b2Vec2>(L, k, strllen(k))
#define luaR_rawsetnil(L, k)   rawsetnil(L, k, strllen(k))
#define luaR_rawsetvec2(L, k)  rawsetvec2(L, k, strllen(k))

namespace b2
{
	b2Vec2 Vec2(lua_State* L, int m, int n);

	void getfield(lua_State* L, const char* field, size_t length);

	template <typename T>
	T gettable(lua_State* L, const char* field, size_t length);

	void rawsetnil(lua_State* L, const char* field, size_t length);

	int rawsetvec2(lua_State* L, const char* field, size_t length);
}

#endif
