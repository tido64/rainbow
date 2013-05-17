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
			int set_texture(lua_State *);
		};
	}
}

#endif
