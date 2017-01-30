// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Label.h"

#include "Lua/lua_Font.h"

NS_RAINBOW_LUA_BEGIN
{
    constexpr bool Label::is_constructible;

    const char Label::class_name[] = "label";

    const luaL_Reg Label::functions[]{
        {"get_color",      &Label::get_color},
        {"set_alignment",  &Label::set_alignment},
        {"set_color",      &Label::set_color},
        {"set_font",       &Label::set_font},
        {"set_position",   &Label::set_position},
        {"set_rotation",   &Label::set_rotation},
        {"set_scale",      &Label::set_scale},
        {"set_text",       &Label::set_text},
        {"move",           &Label::move},
        {nullptr,          nullptr}};

    Label::Label(lua_State* L)
    {
        // rainbow.label(["string"])
        checkargs<nil_or<char*>>(L);

        if (isstring(L, 1))
            label_.set_text(lua_tostring(L, 1));
    }

    int Label::get_color(lua_State* L)
    {
        Label* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const Color& c = self->label_.color();
        lua_pushinteger(L, c.r);
        lua_pushinteger(L, c.g);
        lua_pushinteger(L, c.b);
        lua_pushinteger(L, c.a);
        return 4;
    }

    int Label::set_alignment(lua_State* L)
    {
        // <label>:set_alignment('l' | 'c' | 'r')
        checkargs<Label, char*>(L);

        Label* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const TextAlignment alignment =
            text_alignment_from_char(*lua_tostring(L, 2));
        self->label_.set_alignment(alignment);
        return 0;
    }

    int Label::set_color(lua_State* L)
    {
        // <label>:set_color(r, g, b, a = 255)
        checkargs<Label,
                  lua_Number,
                  lua_Number,
                  lua_Number,
                  nil_or<lua_Number>>(L);

        Label* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const uint8_t r = lua_tointeger(L, 2);
        const uint8_t g = lua_tointeger(L, 3);
        const uint8_t b = lua_tointeger(L, 4);
        const uint8_t a = optinteger(L, 5, 0xff);
        self->label_.set_color(Color{r, g, b, a});
        return 0;
    }

    int Label::set_font(lua_State* L)
    {
        // <label>:set_font(<font>)
        return set1ud<Font>(
            L,
            [](rainbow::Label* label, SharedPtr<FontAtlas> font) {
                label->set_font(std::move(font));
            });
    }

    int Label::set_position(lua_State* L)
    {
        // <label>:set_position(x, y)
        return set1fv(L, [](rainbow::Label* label, const Vec2f& position) {
            label->set_position(position);
        });
    }

    int Label::set_rotation(lua_State* L)
    {
        // <label>:set_rotation(r)
        return set1f(L, [](rainbow::Label* label, float r) {
            label->set_rotation(r);
        });
    }

    int Label::set_scale(lua_State* L)
    {
        // <label>:set_scale(f)
        return set1f(L, [](rainbow::Label* label, float f) {
            label->set_scale(f);
        });
    }

    int Label::set_text(lua_State* L)
    {
        // <label>:set_text("string")
        checkargs<Label, char*>(L);

        Label* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->label_.set_text(lua_tostring(L, 2));
        return 0;
    }

    int Label::move(lua_State* L)
    {
        // <label>:move(x, y)
        return set1fv(L, [](rainbow::Label* label, const Vec2f& delta) {
            label->move(delta);
        });
    }
} NS_RAINBOW_LUA_END
