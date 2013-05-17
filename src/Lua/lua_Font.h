#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "Graphics/FontAtlas.h"
#include "Lua/LuaBind.h"

namespace Rainbow
{
	namespace Lua
	{
		class Font : public Bind<Font, FontAtlas, kBindTypeStrong>
		{
		public:
			Font(lua_State *);
		};
	}
}

#endif
