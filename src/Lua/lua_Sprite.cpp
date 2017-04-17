// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Sprite.h"

using rainbow::SpriteRef;
using rainbow::lua::Sprite;

Sprite::Sprite(lua_State* L)
    : sprite_(*static_cast<SpriteRef*>(lua_touserdata(L, 1)))
{
}

auto Sprite::get_angle(lua_State* L) -> int
{
    return get1f(L, [](const SpriteRef& sprite) {
        return sprite->angle();
    });
}

auto Sprite::get_color(lua_State* L) -> int
{
    return with_self(L, [](Sprite* self, lua_State* L) {
        const Color& c = self->sprite_->color();
        lua_pushinteger(L, c.r);
        lua_pushinteger(L, c.g);
        lua_pushinteger(L, c.b);
        lua_pushinteger(L, c.a);
        return 4;
    });
}

auto Sprite::get_pivot(lua_State* L) -> int
{
    return get1fv(L, [](const SpriteRef& sprite) {
        return sprite->pivot();
    });
}

auto Sprite::get_position(lua_State* L) -> int
{
    return get1fv(L, [](const SpriteRef& sprite) {
        return sprite->position();
    });
}

auto Sprite::get_scale(lua_State* L) -> int
{
    return get1fv(L, [](const SpriteRef& sprite) {
        return sprite->scale();
    });
}

auto Sprite::get_size(lua_State* L) -> int
{
    return with_self(L, [](Sprite* self, lua_State* L) {
        lua_pushnumber(L, self->sprite_->width());
        lua_pushnumber(L, self->sprite_->height());
        return 2;
    });
}

auto Sprite::set_color(lua_State* L) -> int
{
    // <sprite>:set_color(r, g, b, a = 255)
    checkargs<Sprite,  //
              lua_Number,
              lua_Number,
              lua_Number,
              nil_or<lua_Number>>(L);

    return with_self(L, [](Sprite* self, lua_State* L) {
        const Color color(static_cast<uint32_t>(lua_tointeger(L, 2)) & 0xff,
                          static_cast<uint32_t>(lua_tointeger(L, 3)) & 0xff,
                          static_cast<uint32_t>(lua_tointeger(L, 4)) & 0xff,
                          static_cast<uint32_t>(optinteger(L, 5, 0xff)) & 0xff);
        self->sprite_->set_color(color);
        return 0;
    });
}

auto Sprite::set_normal(lua_State* L) -> int
{
    // <sprite>:set_normal(<texture>)
    return set1i(L, [](SpriteRef& sprite, int normal) {
        sprite->set_normal(static_cast<uint32_t>(normal));
    });
}

auto Sprite::set_pivot(lua_State* L) -> int
{
    // <sprite>:set_pivot(x, y)
    return set1fv(L, [](SpriteRef& sprite, const Vec2f& p) {
        sprite->set_pivot(p);
    });
}

auto Sprite::set_position(lua_State* L) -> int
{
    // <sprite>:set_position(x, y)
    return set1fv(L, [](SpriteRef& sprite, const Vec2f& p) {
        sprite->set_position(p);
    });
}

auto Sprite::set_rotation(lua_State* L) -> int
{
    // <sprite>:set_rotation(r)
    return set1f(L, [](SpriteRef& sprite, float r) {
        sprite->set_rotation(r);
    });
}

auto Sprite::set_scale(lua_State* L) -> int
{
    // <sprite>:set_scale(fx, fy = fx)
    checkargs<Sprite, lua_Number, nil_or<lua_Number>>(L);

    return with_self(L, [](Sprite* self, lua_State* L) {
        const float fx = lua_tonumber(L, 2);
        self->sprite_->set_scale(Vec2f(fx, optnumber(L, 3, fx)));
        return 0;
    });
}

auto Sprite::set_texture(lua_State* L) -> int
{
    // <sprite>:set_texture(<texture>)
    return set1i(L, [](SpriteRef& sprite, int texture) {
        sprite->set_texture(static_cast<uint32_t>(texture));
    });
}

auto Sprite::mirror(lua_State* L) -> int
{
    return with_self(L, [](Sprite* self, lua_State*) {
        self->sprite_->mirror();
        return 0;
    });
}

auto Sprite::move(lua_State* L) -> int
{
    // <sprite>:move(x, y)
    return set1fv(L, [](SpriteRef& sprite, const Vec2f& delta) {
        if (delta.is_zero())
            return;

        sprite->move(delta);
    });
}

auto Sprite::rotate(lua_State* L) -> int
{
    // <sprite>:rotate(r)
    return set1f(L, [](SpriteRef& sprite, float r) {
        sprite->rotate(r);
    });
}

LUA_REG_OBJECT(Sprite, "sprite") {
    LUA_REG_OBJECT_FUNC(Sprite, get_angle),
    LUA_REG_OBJECT_FUNC(Sprite, get_color),
    LUA_REG_OBJECT_FUNC(Sprite, get_pivot),
    LUA_REG_OBJECT_FUNC(Sprite, get_position),
    LUA_REG_OBJECT_FUNC(Sprite, get_scale),
    LUA_REG_OBJECT_FUNC(Sprite, get_size),
    LUA_REG_OBJECT_FUNC(Sprite, set_color),
    LUA_REG_OBJECT_FUNC(Sprite, set_normal),
    LUA_REG_OBJECT_FUNC(Sprite, set_pivot),
    LUA_REG_OBJECT_FUNC(Sprite, set_position),
    LUA_REG_OBJECT_FUNC(Sprite, set_rotation),
    LUA_REG_OBJECT_FUNC(Sprite, set_scale),
    LUA_REG_OBJECT_FUNC(Sprite, set_texture),
    LUA_REG_OBJECT_FUNC(Sprite, mirror),
    LUA_REG_OBJECT_FUNC(Sprite, move),
    LUA_REG_OBJECT_FUNC(Sprite, rotate),
    LUA_REG_OBJECT_END()
};
