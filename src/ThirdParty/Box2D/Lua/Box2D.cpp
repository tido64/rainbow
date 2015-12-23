// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Box2D.h"

#include <algorithm>

#include "ThirdParty/Box2D/DebugDraw.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Body.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Contacts/Contact.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"
#include "ThirdParty/Box2D/Lua/Dynamics/World.h"
#include "ThirdParty/Box2D/StableWorld.h"

namespace b2
{
	namespace lua
	{
		void init(lua_State* L)
		{
			lua_createtable(L, 0, 16);
			ShapesInit(L);
			Body::Init(L);
			Fixture::Init(L);
			rainbow::lua::reg<World>(L);
			rainbow::lua::reg<Contact>(L);
			lua_setglobal(L, "b2");

			new DebugDraw();  // Manages its own lifetime.
		}
	}
}
