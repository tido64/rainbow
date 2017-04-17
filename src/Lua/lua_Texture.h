// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_TEXTURE_H_
#define LUA_TEXTURE_H_

#include "Graphics/TextureAtlas.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class Texture : private Object<Texture>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit Texture(lua_State*);

        auto get() const { return texture_; }

    private:
        static auto create(lua_State*) -> int;
        static auto trim(lua_State*) -> int;

        SharedPtr<TextureAtlas> texture_;
    };
}}  // namespace rainbow::lua

#endif
