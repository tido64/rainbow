#ifndef LUA_PLATFORM_H_
#define LUA_PLATFORM_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		namespace Platform
		{
			void init(lua_State *L);

			/// Updates rainbow.platform.screen.
			void update(lua_State *L, const unsigned int width, const unsigned int height);
		}
	}
}

#endif
