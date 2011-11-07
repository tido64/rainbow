#ifdef _WIN32
#	pragma warning(disable : 4244)
#endif

#include "Graphics/Texture.h"
#include "Lua/lua_Texture.h"

const char lua_Texture::class_name[] = "load_texture";
const LuaMachine::Method<lua_Texture> lua_Texture::methods[] = {
	{ "create",  &lua_Texture::create },
	{ "trim",    &lua_Texture::trim },
	{ 0, 0 }
};

lua_Texture::lua_Texture(lua_State *L)
{
	this->t = static_cast<Texture *>(lua_touserdata(L, -1));
}

int lua_Texture::create(lua_State *L)
{
	assert(lua_gettop(L) == 4 || !"Rainbow::Lua::Texture::create takes four parameters");
	lua_pushinteger(L, this->t->create(lua_tointeger(L, 1), lua_tointeger(L, 2), lua_tointeger(L, 3), lua_tointeger(L, 4)));
	return 1;
}

int lua_Texture::trim(lua_State *)
{
	this->t->trim();
	return 0;
}
