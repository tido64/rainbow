#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "Common/Data.h"
#include "Lua/lua_Texture.h"

const char lua_Texture::class_name[] = "texture";
const LuaMachine::Method<lua_Texture> lua_Texture::methods[] = {
	{ "create",  &lua_Texture::create },
	{ "trim",    &lua_Texture::trim },
	{ 0, 0 }
};

lua_Texture::lua_Texture(lua_State *L)
{
	switch (lua_type(L, -1))
	{
		case LUA_TLIGHTUSERDATA:
			this->texture = static_cast<TextureAtlas*>(lua_touserdata(L, -1));
			break;
		case LUA_TSTRING:
			{
				const char *const path = Data::get_path(lua_tolstring(L, -1, nullptr));
				Data tex_data(path);
				Data::free(path);
				if (!tex_data)
					luaL_error(L, "rainbow.texture: Failed to load texture");
				this->texture = new TextureAtlas(tex_data);
				if (!*this->texture)
					luaL_error(L, "rainbow.texture: Failed to create texture");
			}
			break;
		default:
			assert(lua_gettop(L) == 4 || !"Rainbow::Lua syntax: rainbow.texture(<path to texture>)");
			break;
	}
}

int lua_Texture::create(lua_State *L)
{
	assert(lua_gettop(L) == 4 || !"Rainbow::Lua syntax: rainbow.texture:create(x, y, width, height)");

	const int x = lua_tointeger(L, 1);
	const int y = lua_tointeger(L, 2);
	const int w = lua_tointeger(L, 3);
	const int h = lua_tointeger(L, 4);
	lua_pushinteger(L, this->texture->define(x, y, w, h));
	return 1;
}

int lua_Texture::trim(lua_State *)
{
	this->texture->trim();
	return 0;
}
