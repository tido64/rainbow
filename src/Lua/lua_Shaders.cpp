// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/LuaBind.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Shaders.h"

namespace
{
	int diffuse(lua_State *L)
	{
		return Rainbow::Lua::alloc<Rainbow::Lua::Shaders::Diffuse>(L);
	}
}

namespace Rainbow
{
	namespace Lua
	{
		namespace Shaders
		{
			void init(lua_State *L)
			{
				// Initialise "rainbow.shaders" namespace
				lua_pushliteral(L, "shaders");
				lua_createtable(L, 0, 3);

				luaR_rawsetcclosurefield(L, &diffuse, "diffuse");

				wrap<Diffuse>(L, true);

				lua_rawset(L, -3);
			}

			const char Diffuse::class_name[] = "shaders.diffuse";

			const Rainbow::Lua::Method<Diffuse> Diffuse::methods[] = {
				{ "set_cutoff",    &Diffuse::set_cutoff },
				{ "set_radius",    &Diffuse::set_radius },
				{ "set_position",  &Diffuse::set_position },
				{ nullptr, nullptr }
			};

			Diffuse::Diffuse(lua_State *L) : lighting(lua_toboolean(L, 1)) { }

			const Rainbow::Shaders::Diffuse* Diffuse::get() const
			{
				return &this->lighting;
			}

			int Diffuse::id() const
			{
				return this->lighting.id();
			}

			int Diffuse::set_cutoff(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "<diffuse>:set_cutoff(cutoff)");

				this->lighting.set_cutoff(luaR_tonumber(L, 1));
				return 0;
			}

			int Diffuse::set_radius(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1, "<diffuse>:set_radius(radius)");

				this->lighting.set_radius(luaR_tonumber(L, 1));
				return 0;
			}

			int Diffuse::set_position(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 2 || lua_gettop(L) == 3,
				           "<diffuse>:set_position(x, y, z = 0)");

				this->lighting.set_position(luaR_tonumber(L, 1),
				                            luaR_tonumber(L, 2),
				                            luaR_optnumber(L, 3, 100.0));
				return 0;
			}
		}
	}
}
