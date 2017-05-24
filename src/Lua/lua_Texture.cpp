// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Texture.h"

#include <lua.hpp>

using rainbow::lua::Texture;

Texture::Texture(lua_State* L)
{
    // rainbow.texture("/path/to/texture")
    checkargs<char*>(L);

    texture_ = make_shared<TextureAtlas>(lua_tostring(L, 1));
    if (!texture_->is_valid())
        luaL_error(L, "rainbow.texture: Failed to create texture");
}

auto Texture::create(lua_State* L) -> int
{
    // <texture>:create(x, y, width, height)
    checkargs<Texture, lua_Number, lua_Number, lua_Number, lua_Number>(L);

    return with_self(L, [](Texture* self, lua_State* L) {
        const int x = lua_tointeger(L, 2);
        const int y = lua_tointeger(L, 3);
        const int w = lua_tointeger(L, 4);
        const int h = lua_tointeger(L, 5);
        lua_pushinteger(L, self->texture_->add_region(x, y, w, h));
        return 1;
    });
}

auto Texture::trim(lua_State* L) -> int
{
    return with_self(L, [](Texture* self, lua_State*) {
        self->texture_->trim();
        return 0;
    });
}

LUA_REG_OBJECT(Texture, "texture") {
    LUA_REG_OBJECT_FUNC(Texture, create),
    LUA_REG_OBJECT_FUNC(Texture, trim),
    LUA_REG_OBJECT_END()
};
