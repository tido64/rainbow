// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "Graphics/TextureAtlas.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
    class Texture : private Bind<Texture>
    {
    public:
        static constexpr bool is_constructible = true;
        static const char class_name[];
        static const luaL_Reg functions[];

        Texture(lua_State*);

        SharedPtr<TextureAtlas> get() const;

    private:
        static int create(lua_State*);
        static int trim(lua_State*);

        SharedPtr<TextureAtlas> texture_;
    };
} NS_RAINBOW_LUA_END

#endif
