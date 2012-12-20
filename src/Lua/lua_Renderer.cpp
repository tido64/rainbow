#include <cstring>

#include <lua.hpp>

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Lua/lua_Renderer.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Renderer
		{
			void init(lua_State *L)
			{
				lua_createtable(L, 0, 4);
				lua_pushvalue(L, -1);
				lua_setfield(L, -3, "renderer");

				int max_texture_size;
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
				lua_pushinteger(L, max_texture_size);
				lua_setfield(L, -2, "max_texture_size");

				const char *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

				lua_pushboolean(L, strstr(extensions, "GL_IMG_texture_compression_pvrtc") != nullptr);
				lua_setfield(L, -2, "supports_pvrtc");

				lua_pop(L, 1);
			}
		}
	}
}
