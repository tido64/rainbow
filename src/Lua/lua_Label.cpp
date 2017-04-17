// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Label.h"

#include "Lua/lua_Font.h"

using rainbow::lua::Label;

Label::Label(lua_State* L)
{
    // rainbow.label(["string"])
    checkargs<nil_or<char*>>(L);

    if (isstring(L, 1))
        label_.set_text(lua_tostring(L, 1));
}

auto Label::get_color(lua_State* L) -> int
{
    return with_self(L, [](Label* self, lua_State* L) {
        const Color& c = self->label_.color();
        lua_pushinteger(L, c.r);
        lua_pushinteger(L, c.g);
        lua_pushinteger(L, c.b);
        lua_pushinteger(L, c.a);
        return 4;
    });
}

auto Label::set_alignment(lua_State* L) -> int
{
    // <label>:set_alignment('l' | 'c' | 'r')
    checkargs<Label, char*>(L);

    return with_self(L, [](Label* self, lua_State* L) {
        const TextAlignment alignment =
            text_alignment_from_char(*lua_tostring(L, 2));
        self->label_.set_alignment(alignment);
        return 0;
    });
}

auto Label::set_color(lua_State* L) -> int
{
    // <label>:set_color(r, g, b, a = 255)
    checkargs<Label,  //
              lua_Number,
              lua_Number,
              lua_Number,
              nil_or<lua_Number>>(L);

    return with_self(L, [](Label* self, lua_State* L) {
        const uint8_t r = lua_tointeger(L, 2);
        const uint8_t g = lua_tointeger(L, 3);
        const uint8_t b = lua_tointeger(L, 4);
        const uint8_t a = optinteger(L, 5, 0xff);
        self->label_.set_color(Color{r, g, b, a});
        return 0;
    });
}

auto Label::set_font(lua_State* L) -> int
{
    // <label>:set_font(<font>)
    return set1ud<Font>(
        L,
        [](rainbow::Label* label, SharedPtr<FontAtlas> font) {
            label->set_font(std::move(font));
        });
}

auto Label::set_position(lua_State* L) -> int
{
    // <label>:set_position(x, y)
    return set1fv(L, [](rainbow::Label* label, const Vec2f& position) {
        label->set_position(position);
    });
}

auto Label::set_rotation(lua_State* L) -> int
{
    // <label>:set_rotation(r)
    return set1f(L, [](rainbow::Label* label, float r) {
        label->set_rotation(r);
    });
}

auto Label::set_scale(lua_State* L) -> int
{
    // <label>:set_scale(f)
    return set1f(L, [](rainbow::Label* label, float f) {
        label->set_scale(f);
    });
}

auto Label::set_text(lua_State* L) -> int
{
    // <label>:set_text("string")
    checkargs<Label, char*>(L);

    return with_self(L, [](Label* self, lua_State* L) {
        self->label_.set_text(lua_tostring(L, 2));
        return 0;
    });
}

auto Label::move(lua_State* L) -> int
{
    // <label>:move(x, y)
    return set1fv(L, [](rainbow::Label* label, const Vec2f& delta) {
        label->move(delta);
    });
}

LUA_REG_OBJECT(Label, "label") {
    LUA_REG_OBJECT_FUNC(Label, get_color),
    LUA_REG_OBJECT_FUNC(Label, set_alignment),
    LUA_REG_OBJECT_FUNC(Label, set_color),
    LUA_REG_OBJECT_FUNC(Label, set_font),
    LUA_REG_OBJECT_FUNC(Label, set_position),
    LUA_REG_OBJECT_FUNC(Label, set_rotation),
    LUA_REG_OBJECT_FUNC(Label, set_scale),
    LUA_REG_OBJECT_FUNC(Label, set_text),
    LUA_REG_OBJECT_FUNC(Label, move),
    LUA_REG_OBJECT_END()
};
