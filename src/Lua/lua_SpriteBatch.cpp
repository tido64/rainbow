// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_SpriteBatch.h"

#include "Lua/lua_Sprite.h"
#include "Lua/lua_Texture.h"

using rainbow::lua::SpriteBatch;

SpriteBatch::SpriteBatch(lua_State* L)
    : batch_(rainbow::lua::optinteger(L, 1, 4))
{
    checkargs<lua_Number>(L);
}

auto SpriteBatch::add(lua_State* L) -> int
{
    LOGW("<spritebatch>:add() is deprecated, use <spritebatch>:create_sprite() "
         "and <sprite>:set_texture() instead.");

    // <spritebatch>:add(x, y, width, height)
    checkargs<SpriteBatch,  //
              lua_Number,
              lua_Number,
              lua_Number,
              lua_Number>(L);

    return with_self(L, [](SpriteBatch* self, lua_State* L) {
        const int x = lua_tointeger(L, 2);
        const int y = lua_tointeger(L, 3);
        const int w = lua_tointeger(L, 4);
        const int h = lua_tointeger(L, 5);

        lua_settop(L, 0);
        auto sprite = self->batch_.add(x, y, w, h);
        lua_pushlightuserdata(L, &sprite);
        return alloc<Sprite>(L);
    });
}

auto SpriteBatch::create_sprite(lua_State* L) -> int
{
    // <spritebatch>:create_sprite(width, height)
    checkargs<SpriteBatch, lua_Number, lua_Number>(L);

    return with_self(L, [](SpriteBatch* self, lua_State* L) {
        const int w = lua_tointeger(L, 2);
        const int h = lua_tointeger(L, 3);

        lua_settop(L, 0);
        auto sprite = self->batch_.create_sprite(w, h);
        lua_pushlightuserdata(L, &sprite);
        return alloc<Sprite>(L);
    });
}

auto SpriteBatch::set_normal(lua_State* L) -> int
{
    // <spritebatch>:set_normal(<texture>)
    return set1ud<Texture>(
        L,
        [](rainbow::SpriteBatch* batch, SharedPtr<TextureAtlas> atlas) {
            batch->set_normal(std::move(atlas));
        });
}

auto SpriteBatch::set_texture(lua_State* L) -> int
{
    // <spritebatch>:set_texture(<texture>)
    return set1ud<Texture>(
        L,
        [](rainbow::SpriteBatch* batch, SharedPtr<TextureAtlas> atlas) {
            batch->set_texture(std::move(atlas));
        });
}

LUA_REG_OBJECT(SpriteBatch, "spritebatch") {
    LUA_REG_OBJECT_FUNC(SpriteBatch, add),
    LUA_REG_OBJECT_FUNC(SpriteBatch, create_sprite),
    LUA_REG_OBJECT_FUNC(SpriteBatch, set_normal),
    LUA_REG_OBJECT_FUNC(SpriteBatch, set_texture),
    LUA_REG_OBJECT_END()
};
