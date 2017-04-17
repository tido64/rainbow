// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_SHADERS_H_
#define LUA_SHADERS_H_

#include "Lua/LuaMacros.h"

struct lua_State;

namespace rainbow { namespace lua
{
    class Shader
    {
    public:
        virtual auto id() const -> int = 0;

    protected:
        ~Shader() {}
    };
}}  // rainbow::lua

namespace rainbow { namespace lua { namespace shaders
{
    void init(lua_State*);
}}}  // rainbow::lua::shaders

#endif
