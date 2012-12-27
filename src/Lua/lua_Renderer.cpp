#include <cstring>

#include <lua.hpp>

#include "Graphics/OpenGL.h"
#include "Graphics/Renderer.h"
#include "Graphics/TextureManager.h"
#include "Lua/lua_Renderer.h"

namespace Rainbow
{
	namespace Lua
	{
		namespace Renderer
		{
			void init(lua_State *L)
			{
				// Initialise "rainbow.renderer" namespace
				lua_createtable(L, 0, 4);

				int max_texture_size;
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
				lua_pushinteger(L, max_texture_size);
				lua_setfield(L, -2, "max_texture_size");

				const char *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

				lua_pushboolean(L, strstr(extensions, "GL_IMG_texture_compression_pvrtc") != nullptr);
				lua_setfield(L, -2, "supports_pvrtc");

				lua_pushcclosure(L, &set_clear_color, 0);
				lua_setfield(L, -2, "set_clear_color");
				lua_pushcclosure(L, &set_filter, 0);
				lua_setfield(L, -2, "set_filter");

				lua_setfield(L, -2, "renderer");

				// Initialise "gl" namespace
				lua_createtable(L, 0, 2);
				lua_pushinteger(L, GL_NEAREST);
				lua_setfield(L, -2, "NEAREST");
				lua_pushinteger(L, GL_LINEAR);
				lua_setfield(L, -2, "LINEAR");
				lua_setglobal(L, "gl");
			}

			int set_clear_color(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 3,
				           "rainbow.renderer.set_clear_color(0xrr, 0xgg, 0xbb)");

				const float r = lua_tointeger(L, 1) / 255;
				const float g = lua_tointeger(L, 2) / 255;
				const float b = lua_tointeger(L, 3) / 255;
				glClearColor(r, g, b, 1.0f);
				return 0;
			}

			int set_filter(lua_State *L)
			{
				LUA_ASSERT(lua_gettop(L) == 1,
				           "rainbow.renderer.set_filter(filter)");

				const int filter = lua_tointeger(L, 1);
				LUA_CHECK(L, filter == GL_NEAREST || filter == GL_LINEAR,
				          "Invalid texture filter");
				TextureManager::Instance().set_filter(filter);
				return 0;
			}
		}
	}
}
