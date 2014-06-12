// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "Graphics/TextureAtlas.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
	class Texture : public Bind<Texture>
	{
		friend Bind;

	public:
		Texture(lua_State *);

		inline TextureAtlas* get();

	private:
		static int create(lua_State *);
		static int trim(lua_State *);

		SharedPtr<TextureAtlas> texture_;
	};

	TextureAtlas* Texture::get()
	{
		return texture_.get();
	}
} NS_RAINBOW_LUA_END

#endif
