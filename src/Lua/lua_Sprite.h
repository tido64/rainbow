// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Lua/LuaBind.h"

struct lua_State;
class Sprite;
class SpriteBatch;

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

			::Sprite* get() const;

		private:
			const unsigned int id;
			const SpriteBatch *batch;

			int get_angle(lua_State *);
			int get_color(lua_State *);
			int get_position(lua_State *);
			int set_color(lua_State *);
			int set_normal(lua_State *);
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
