#include "Common/Random.h"
#include "Lua/LuaHelper.h"
#include "Lua/lua_Random.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Random
		{
			namespace
			{
				int random(lua_State *L)
				{
					lua_Number r = 0.0f;
					switch (lua_gettop(L))
					{
						case 1:
							r = ::Random::next<lua_Number>(luaR_tonumber(L, 1));
							break;
						case 2:
							r = ::Random::next<lua_Number>(
									luaR_tonumber(L, 1), luaR_tonumber(L, 2));
							break;
						default:
							r = ::Random::next();
							break;
					}
					lua_pushnumber(L, r);
					return 1;
				}

				int seed(lua_State *L)
				{
					::Random::seed(luaR_optinteger(L, 1, 0));
					return 0;
				}
			}

			void init(lua_State *L)
			{
				luaR_rawsetcclosurefield(L, &random, "random");
				luaR_rawsetcclosurefield(L, &seed, "seed");
			}
		}
	}
}
