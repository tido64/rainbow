#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "Graphics/TextureAtlas.h"
#include "Lua/LuaBind.h"

namespace Rainbow
{
	namespace Lua
	{
		class Texture : public Bind<Texture, TextureAtlas, kBindTypeStrong>
		{
			friend class Bind<Texture, TextureAtlas, kBindTypeStrong>;

		public:
			Texture(lua_State *);

		private:
			int create(lua_State *);
			int trim(lua_State *);
		};
	}
}

#endif
