#ifndef LUA_RENDERER_H_
#define LUA_RENDERER_H_

namespace Rainbow
{
	namespace Lua
	{
		namespace Renderer
		{
			void init(lua_State *);

			int set_clear_color(lua_State *);
			int set_filter(lua_State *);
		}
	}
}

#endif
