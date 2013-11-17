// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_PHYSICS

#include <Box2D/Box2D.h>

#include "Lua/LuaHelper.h"

namespace b2
{
	float ptm_ratio = 32.0f;
	int g_body_list = -1;     ///< List of Lua::Body objects for retrieval.
}

#include "Physics/lua_b2Draw.h"
#include "Physics/lua_b2Shape.h"
#include "Physics/lua_b2Fixture.h"
#include "Physics/lua_b2Body.h"
#include "Physics/lua_b2Contact.h"
#include "Physics/lua_b2World.h"

namespace b2
{
	namespace Lua
	{
		namespace
		{
			int set_ptm_ratio(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "b2.SetPTMRatio(r)");

				ptm_ratio = luaR_tonumber(L, 1);
				return 0;
			}
		}

		void Init(lua_State *L)
		{
			lua_createtable(L, 0, 0);
			g_body_list = luaL_ref(L, LUA_REGISTRYINDEX);
			g_debug_data = new DebugData();

			lua_createtable(L, 0, 16);

			luaR_rawsetcclosurefield(L, &set_ptm_ratio, "SetPTMRatio");

			// b2CircleShape
			luaR_rawsetcclosurefield(L, &CircleShape, "CircleShape");

			// b2PolygonShape
			Rainbow::Lua::wrap<PolygonShape>(L);

			// b2BodyType
			luaR_rawsetfield(L, lua_pushinteger, b2_staticBody, "staticBody");
			luaR_rawsetfield(L, lua_pushinteger, b2_kinematicBody, "kinematicBody");
			luaR_rawsetfield(L, lua_pushinteger, b2_dynamicBody, "dynamicBody");

			// b2BodyDef
			luaR_rawsetcclosurefield(L, &BodyDef, "BodyDef");

			// b2Body
			Rainbow::Lua::wrap<Body>(L);

			// b2FixtureDef
			luaR_rawsetcclosurefield(L, &FixtureDef, "FixtureDef");

			// b2Fixture
			Rainbow::Lua::wrap<Fixture>(L, true);

			// b2World
			Rainbow::Lua::wrap<World>(L);

			// b2Contact
			Rainbow::Lua::wrap<Contact>(L, true);

			lua_setglobal(L, "b2");
		}
	}
}

#endif
