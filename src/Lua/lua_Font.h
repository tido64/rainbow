// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "Graphics/FontAtlas.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua
{
    class Font : private Object<Font>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit Font(lua_State*);

        auto get() const { return font_; }

    private:
        SharedPtr<FontAtlas> font_;
    };
}}  // namespace rainbow::lua

#endif
