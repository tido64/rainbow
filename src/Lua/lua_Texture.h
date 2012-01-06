#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "LuaMachine.h"
#include "Graphics/Texture.h"

class lua_Texture
{
public:
	static const char class_name[];
	static const LuaMachine::Method<lua_Texture> methods[];

	lua_Texture(lua_State *);

	Texture* raw_ptr() const;

	int create(lua_State *);
	int trim(lua_State *);

private:
	SmartPtr<Texture> texture;
};

inline Texture* lua_Texture::raw_ptr() const
{
	return this->texture.raw_ptr();
}

#endif
