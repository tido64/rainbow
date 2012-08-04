#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

#include <lua.hpp>

#include "Platform.h"

namespace Rainbow
{
	namespace Lua
	{
		class Platform
		{
			friend void init(lua_State *);

		public:
			/// Updates rainbow.platform.screen
			static void update(lua_State *L, const unsigned int width, const unsigned int height)
			{
				lua_getglobal(L, "rainbow");
				lua_getfield(L, -1, "platform");
				lua_getfield(L, -1, "screen");

				lua_pushnumber(L, width);
				lua_setfield(L, -2, "width");

				lua_pushnumber(L, height);
				lua_setfield(L, -2, "height");

				lua_pop(L, 3);
			}

		private:
			static void init(lua_State *L)
			{
				lua_createtable(L, 0, 4);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "platform");

				lua_pushboolean(L, RAINBOW_ACCELERATED);
				lua_setfield(L, -2, "accelerometer");

				// rainbow.platform.screen
				lua_createtable(L, 0, 2);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "screen");

				lua_pushboolean(L, RAINBOW_TOUCHED);
				lua_setfield(L, -2, "touch");

				lua_pop(L, 2);
			}

			Platform();
			Platform(const Platform &);
			Platform& operator=(const Platform &);
		};
	}
}

#endif
