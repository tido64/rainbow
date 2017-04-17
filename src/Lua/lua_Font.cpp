// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Font.h"

using rainbow::lua::Font;

Font::Font(lua_State* L)
{
    // rainbow.font("path/to/fontface", font_size)
    checkargs<char*, lua_Number>(L);

    font_ = make_shared<FontAtlas>(lua_tostring(L, 1), lua_tonumber(L, 2));
    if (!font_->is_valid())
        luaL_error(L, "rainbow.font: Failed to create font texture");
}

LUA_REG_OBJECT(Font, "font") {
    LUA_REG_OBJECT_END()
};
