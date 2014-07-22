// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_PHYSICS

#include <Box2D/Box2D.h>

#include "Lua/LuaBind.h"
#include "ThirdParty/Box2D/b2LuaHelper.h"

using Rainbow::Lua::Bind;

namespace b2
{
	float g_ptm_ratio = 32.0f;
	int g_body_list = -1;     ///< List of Lua::Body objects for retrieval.
}

#include "ThirdParty/Box2D/lua_b2Draw.h"
#include "ThirdParty/Box2D/lua_b2Shape.h"
#include "ThirdParty/Box2D/lua_b2Fixture.h"
#include "ThirdParty/Box2D/lua_b2Body.h"
#include "ThirdParty/Box2D/lua_b2Contact.h"
#include "ThirdParty/Box2D/lua_b2World.h"

NS_B2_LUA_BEGIN
{
	namespace
	{
		int set_ptm_ratio(lua_State *L)
		{
			// b2.SetPTMRatio(r)
			Rainbow::Lua::Argument<lua_Number>::is_required(L, 1);

			g_ptm_ratio = lua_tonumber(L, 1);
			return 0;
		}
	}

	void Init(lua_State *L)
	{
		lua_createtable(L, 0, 0);
		g_body_list = luaL_ref(L, LUA_REGISTRYINDEX);
		g_debug_data = new DebugData();

		lua_createtable(L, 0, 16);

		luaR_rawsetcfunction(L, "SetPTMRatio", &set_ptm_ratio);

		// b2CircleShape
		luaR_rawsetcfunction(L, "CircleShape", &CircleShape);

		// b2PolygonShape
		Rainbow::Lua::reg<PolygonShape>(L);

		// b2BodyType
		luaR_rawsetinteger(L, "staticBody", b2_staticBody);
		luaR_rawsetinteger(L, "kinematicBody", b2_kinematicBody);
		luaR_rawsetinteger(L, "dynamicBody", b2_dynamicBody);

		// b2BodyDef
		luaR_rawsetcfunction(L, "BodyDef", &BodyDef);

		// b2Body
		Rainbow::Lua::reg<Body>(L);

		// b2FixtureDef
		luaR_rawsetcfunction(L, "FixtureDef", &FixtureDef);

		// b2Fixture
		Rainbow::Lua::reg<Fixture>(L);

		// b2World
		Rainbow::Lua::reg<World>(L);

		// b2Contact
		Rainbow::Lua::reg<Contact>(L);

		lua_setglobal(L, "b2");
	}
} NS_B2_LUA_END

#endif
