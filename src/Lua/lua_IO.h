#ifndef LUA_IO_H_
#define LUA_IO_H_

struct lua_State;

namespace Rainbow
{
	namespace Lua
	{
		namespace IO
		{
			void init(lua_State *);

			int load(lua_State *);
			int save(lua_State *);
		}
	}
}

#endif
