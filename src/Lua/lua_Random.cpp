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
					Number r = 0.0f;
					switch (lua_gettop(L))
					{
						case 1:
							r = ::Random::Instance().next<Number>(luaR_tonumber(L, 1));
							break;
						case 2:
							r = luaR_tonumber(L, 1);
							r = ::Random::Instance().next<Number>(r, luaR_tonumber(L, 2));
							break;
						default:
							r = ::Random::Instance().next();
							break;
					}
					lua_pushnumber(L, r);
					return 1;
				}
			}

			void init(lua_State *L)
			{
				luaR_rawsetcclosurefield(L, &random, "random");
			}
		}
	}
}
