#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "LuaMachine.h"
#include "Graphics/SpriteBatch.h"

namespace Rainbow
{
	namespace Lua
	{
		class SpriteBatch : public ::SpriteBatch
		{
		public:
			static const char class_name[];
			static const LuaMachine::Method<SpriteBatch> methods[];

			inline SpriteBatch(lua_State *L);

			inline ::SpriteBatch* raw_ptr();

			int add(lua_State *);
			int set_texture(lua_State *);
		};

		SpriteBatch::SpriteBatch(lua_State *L) :
			::SpriteBatch(lua_tointeger(L, 1)) { }

		::SpriteBatch* SpriteBatch::raw_ptr()
		{
			return this;
		}
	}
}

#endif
