// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "Graphics/TextureAtlas.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
	class Texture : public Bind<Texture, TextureAtlas, kBindTypeStrong>
	{
		friend Texture::Bind;

	public:
		Texture(lua_State *);

	private:
		int create(lua_State *);
		int trim(lua_State *);
	};
} NS_RAINBOW_LUA_END

#endif
