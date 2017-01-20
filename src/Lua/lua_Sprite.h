// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SPRITE_H_
#define LUA_SPRITE_H_

#include "Graphics/Sprite.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
    class Sprite : private Bind<Sprite>
    {
    public:
        static constexpr bool is_constructible = false;
        static const char class_name[];
        static const luaL_Reg functions[];

        Sprite(lua_State*);

        auto get() -> SpriteRef& { return sprite_; }

    private:
        static int get_angle(lua_State*);
        static int get_color(lua_State*);
        static int get_pivot(lua_State*);
        static int get_position(lua_State*);
        static int get_scale(lua_State*);
        static int get_size(lua_State*);
        static int set_color(lua_State*);
        static int set_normal(lua_State*);
        static int set_pivot(lua_State*);
        static int set_position(lua_State*);
        static int set_rotation(lua_State*);
        static int set_scale(lua_State*);
        static int set_texture(lua_State*);

        static int mirror(lua_State*);
        static int move(lua_State*);
        static int rotate(lua_State*);

        SpriteRef sprite_;
    };
} NS_RAINBOW_LUA_END

#endif
