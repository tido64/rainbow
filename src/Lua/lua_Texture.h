#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "LuaMachine.h"
#include "Graphics/TextureAtlas.h"

namespace Rainbow
{
	namespace Lua
	{
		class Texture
		{
		public:
			static const char class_name[];
			static const LuaMachine::Method<Texture> methods[];

			Texture(lua_State *);

			inline TextureAtlas* raw_ptr() const;

			int create(lua_State *);
			int trim(lua_State *);

		private:
			SharedPtr<TextureAtlas> texture;
		};

		TextureAtlas* Texture::raw_ptr() const
		{
			return this->texture.raw_ptr();
		}
	}
}

#endif
