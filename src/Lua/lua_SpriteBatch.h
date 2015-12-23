// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITEBATCH_H_
#define LUA_SPRITEBATCH_H_

#include "Graphics/SpriteBatch.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
	class SpriteBatch : public Bind<SpriteBatch>
	{
		friend Bind;

	public:
		SpriteBatch(lua_State* L);

		::SpriteBatch* get() { return &batch_; }

	private:
		static int add(lua_State*);
		static int create_sprite(lua_State*);
		static int set_normal(lua_State*);
		static int set_texture(lua_State*);

		::SpriteBatch batch_;
	};
} NS_RAINBOW_LUA_END

#endif
