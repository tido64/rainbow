// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "Graphics/SpriteBatch.h"
#include "Lua/LuaBind.h"

namespace Rainbow
{
	namespace Lua
	{
		class SpriteBatch :
			public ::SpriteBatch,
			public Bind<SpriteBatch, ::SpriteBatch, kBindTypeDerived>
		{
			friend class Bind<SpriteBatch, ::SpriteBatch, kBindTypeDerived>;

		public:
			SpriteBatch(lua_State *L);

		private:
			int add(lua_State *);
			int create_sprite(lua_State *);
			int set_normal(lua_State *);
			int set_texture(lua_State *);
		};
	}
}

#endif
