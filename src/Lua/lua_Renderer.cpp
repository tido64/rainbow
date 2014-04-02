// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <lua.hpp>

#include "Graphics/Renderer.h"
#include "Graphics/ShaderManager.h"
#include "Graphics/TextureManager.h"
#include "Lua/lua_Renderer.h"

namespace
{
	int set_clear_color(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1) &&
		           lua_isnumber(L, 2) &&
		           lua_isnumber(L, 3),
		           "rainbow.renderer.set_clear_color(0xrr, 0xgg, 0xbb)");

		const float r = lua_tonumber(L, 1) / 255.0f;
		const float g = lua_tonumber(L, 2) / 255.0f;
		const float b = lua_tonumber(L, 3) / 255.0f;
		glClearColor(r, g, b, 1.0f);
		return 0;
	}

	int set_filter(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1), "rainbow.renderer.set_filter(filter)");

		const int filter = lua_tointeger(L, 1);
		LUA_CHECK(L, filter == GL_NEAREST || filter == GL_LINEAR,
		          "Invalid texture filter");
		TextureManager::Instance->set_filter(filter);
		return 0;
	}

	int set_projection(lua_State *L)
	{
		LUA_ASSERT(lua_isnumber(L, 1) &&
		           lua_isnumber(L, 2) &&
		           lua_isnumber(L, 3) &&
		           lua_isnumber(L, 4),
		           "rainbow.renderer.set_projection(left, right, bottom, top)");

		ShaderManager::Instance->set_projection(
		    lua_tonumber(L, 1), lua_tonumber(L, 2),
		    lua_tonumber(L, 3), lua_tonumber(L, 4));
		return 0;
	}
}

NS_RAINBOW_LUA_MODULE_BEGIN(Renderer)
{
	void init(lua_State *L)
	{
		// Initialise "rainbow.renderer" namespace
		lua_pushliteral(L, "renderer");
		lua_createtable(L, 0, 5);

		int max_texture_size;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
		luaR_rawsetfield(L, lua_pushinteger, max_texture_size, "max_texture_size");

		const char *extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
		R_ASSERT(extensions, "OpenGL context not set up");
		bool support = strstr(extensions, "GL_IMG_texture_compression_pvrtc") != nullptr;
		luaR_rawsetfield(L, lua_pushboolean, support, "supports_pvrtc");

		luaR_rawsetcclosurefield(L, &set_clear_color, "set_clear_color");
		luaR_rawsetcclosurefield(L, &set_filter, "set_filter");
		luaR_rawsetcclosurefield(L, &set_projection, "set_projection");

		lua_rawset(L, -3);

		// Initialise "gl" namespace
		lua_createtable(L, 0, 2);
		luaR_rawsetfield(L, lua_pushinteger, GL_NEAREST, "NEAREST");
		luaR_rawsetfield(L, lua_pushinteger, GL_LINEAR, "LINEAR");
		lua_setglobal(L, "gl");
	}
} NS_RAINBOW_LUA_MODULE_END(Renderer)
