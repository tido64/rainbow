// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Font.h"

NS_RAINBOW_LUA_BEGIN
{
    constexpr bool Font::is_constructible;

    const char Font::class_name[] = "font";

    const luaL_Reg Font::functions[]{{nullptr, nullptr}};

    Font::Font(lua_State* L)
    {
        // rainbow.font("path/to/fontface", font_size)
        checkargs<char*, lua_Number>(L);

        font_ = make_shared<FontAtlas>(lua_tostring(L, 1), lua_tonumber(L, 2));
        if (!font_->is_valid())
            luaL_error(L, "rainbow.font: Failed to create font texture");
    }

    SharedPtr<FontAtlas> Font::get() const { return font_; }
} NS_RAINBOW_LUA_END
