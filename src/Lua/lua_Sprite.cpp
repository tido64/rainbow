// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Sprite.h"

using uint_t = unsigned int;

NS_RAINBOW_LUA_BEGIN
{
    template <>
    const char Sprite::Bind::class_name[] = "sprite";

    template <>
    const bool Sprite::Bind::is_constructible = false;

    template <>
    const luaL_Reg Sprite::Bind::functions[] = {
        {"get_angle",     &Sprite::get_angle},
        {"get_color",     &Sprite::get_color},
        {"get_pivot",     &Sprite::get_pivot},
        {"get_position",  &Sprite::get_position},
        {"get_scale",     &Sprite::get_scale},
        {"get_size",      &Sprite::get_size},
        {"set_color",     &Sprite::set_color},
        {"set_normal",    &Sprite::set_normal},
        {"set_pivot",     &Sprite::set_pivot},
        {"set_position",  &Sprite::set_position},
        {"set_rotation",  &Sprite::set_rotation},
        {"set_scale",     &Sprite::set_scale},
        {"set_texture",   &Sprite::set_texture},
        {"mirror",        &Sprite::mirror},
        {"move",          &Sprite::move},
        {"rotate",        &Sprite::rotate},
        {nullptr,         nullptr}};

    Sprite::Sprite(lua_State* L)
        : sprite_(*static_cast<SpriteRef*>(lua_touserdata(L, 1))) {}

    int Sprite::get_angle(lua_State* L)
    {
        return get1f(L, [](const SpriteRef& sprite) {
            return sprite->angle();
        });
    }

    int Sprite::get_color(lua_State* L)
    {
        Sprite* self = Bind::self(L);
        if (!self)
            return 0;

        const Colorb& c = self->sprite_->color();
        lua_pushinteger(L, c.r);
        lua_pushinteger(L, c.g);
        lua_pushinteger(L, c.b);
        lua_pushinteger(L, c.a);
        return 4;
    }

    int Sprite::get_pivot(lua_State* L)
    {
        return get1fv(L, [](const SpriteRef& sprite) {
            return sprite->pivot();
        });
    }

    int Sprite::get_position(lua_State* L)
    {
        return get1fv(L, [](const SpriteRef& sprite) {
            return sprite->position();
        });
    }

    int Sprite::get_scale(lua_State* L)
    {
        return get1fv(L, [](const SpriteRef& sprite) {
            return sprite->scale();
        });
    }

    int Sprite::get_size(lua_State* L)
    {
        Sprite* self = Bind::self(L);
        if (!self)
            return 0;

        lua_pushnumber(L, self->sprite_->width());
        lua_pushnumber(L, self->sprite_->height());
        return 2;
    }

    int Sprite::set_color(lua_State* L)
    {
        // <sprite>:set_color(r, g, b, a = 255)
        Argument<lua_Number>::is_required(L, 2);
        Argument<lua_Number>::is_required(L, 3);
        Argument<lua_Number>::is_required(L, 4);
        Argument<lua_Number>::is_optional(L, 5);

        Sprite* self = Bind::self(L);
        if (!self)
            return 0;

        const Colorb color(static_cast<uint_t>(lua_tointeger(L, 2)) & 0xff,
                           static_cast<uint_t>(lua_tointeger(L, 3)) & 0xff,
                           static_cast<uint_t>(lua_tointeger(L, 4)) & 0xff,
                           static_cast<uint_t>(optinteger(L, 5, 0xff)) & 0xff);
        self->sprite_->set_color(color);
        return 0;
    }

    int Sprite::set_normal(lua_State* L)
    {
        // <sprite>:set_normal(<texture>)
        return set1i(L, [](const SpriteRef& sprite, int normal) {
            sprite->set_normal(static_cast<uint_t>(normal));
        });
    }

    int Sprite::set_pivot(lua_State* L)
    {
        // <sprite>:set_pivot(x, y)
        return set1fv(L, [](const SpriteRef& sprite, const Vec2f& p) {
            sprite->set_pivot(p);
        });
    }

    int Sprite::set_position(lua_State* L)
    {
        // <sprite>:set_position(x, y)
        return set1fv(L, [](const SpriteRef& sprite, const Vec2f& p) {
            sprite->set_position(p);
        });
    }

    int Sprite::set_rotation(lua_State* L)
    {
        // <sprite>:set_rotation(r)
        return set1f(L, [](const SpriteRef& sprite, float r) {
            sprite->set_rotation(r);
        });
    }

    int Sprite::set_scale(lua_State* L)
    {
        // <sprite>:set_scale(fx, fy = fx)
        Argument<lua_Number>::is_required(L, 2);
        Argument<lua_Number>::is_optional(L, 3);

        Sprite* self = Bind::self(L);
        if (!self)
            return 0;

        const float fx = lua_tonumber(L, 2);
        self->sprite_->set_scale(Vec2f(fx, optnumber(L, 3, fx)));
        return 0;
    }

    int Sprite::set_texture(lua_State* L)
    {
        // <sprite>:set_texture(<texture>)
        return set1i(L, [](const SpriteRef& sprite, int texture) {
            sprite->set_texture(static_cast<uint_t>(texture));
        });
    }

    int Sprite::mirror(lua_State* L)
    {
        Sprite* self = Bind::self(L);
        if (!self)
            return 0;

        self->sprite_->mirror();
        return 0;
    }

    int Sprite::move(lua_State* L)
    {
        // <sprite>:move(x, y)
        return set1fv(L, [](const SpriteRef& sprite, const Vec2f& delta) {
            if (delta.is_zero())
                return;

            sprite->move(delta);
        });
    }

    int Sprite::rotate(lua_State* L)
    {
        // <sprite>:rotate(r)
        return set1f(L, [](const SpriteRef& sprite, float r) {
            sprite->rotate(r);
        });
    }
} NS_RAINBOW_LUA_END
