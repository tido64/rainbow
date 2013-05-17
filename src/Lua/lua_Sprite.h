#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Lua/LuaBind.h"

struct lua_State;
class  Sprite;

namespace Rainbow
{
	namespace Lua
	{
		class Sprite : public Bind<Sprite, ::Sprite, kBindTypeWeak>
		{
			friend class Bind<Sprite, ::Sprite, kBindTypeWeak>;

		public:
			Sprite(lua_State *);

		private:
			int get_angle(lua_State *);
			int get_color(lua_State *);
			int get_position(lua_State *);
			int set_color(lua_State *);
			int set_pivot(lua_State *);
			int set_position(lua_State *);
			int set_rotation(lua_State *);
			int set_scale(lua_State *);
			int set_texture(lua_State *);

			int mirror(lua_State *);
			int move(lua_State *);
			int rotate(lua_State *);
		};
	}
}

#endif
