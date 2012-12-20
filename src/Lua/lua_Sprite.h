#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Lua/LuaHelper.h"

struct lua_State;
class  Sprite;

namespace Rainbow
{
	namespace Lua
	{
		class Sprite
		{
		public:
			static const char class_name[];
			static const Method<Sprite> methods[];

			Sprite(lua_State *);

			inline ::Sprite* raw_ptr() const;

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

		private:
			::Sprite *sprite;
		};

		::Sprite* Sprite::raw_ptr() const
		{
			return this->sprite;
		}
	}
}

#endif
