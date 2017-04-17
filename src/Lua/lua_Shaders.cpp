// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Shaders.h"

#include "Graphics/Shaders/Diffuse.h"
#include "Lua/Object.h"

namespace rainbow { namespace lua { namespace shaders
{
    class Diffuse final : public Shader, private Object<Diffuse>
    {
    public:
        LUA_REG_OBJECT_PROPS(false)

        explicit Diffuse(lua_State* L) : lighting_(toboolean(L, 1)) {}

        auto get() const { return &lighting_; }

        auto id() const -> int override { return lighting_.id(); }

    private:
        static auto set_cutoff(lua_State*) -> int;
        static auto set_radius(lua_State*) -> int;
        static auto set_position(lua_State*) -> int;

        rainbow::shaders::Diffuse lighting_;
    };

    auto diffuse(lua_State* L) { return alloc<Diffuse>(L); }

    void init(lua_State* L)
    {
        // Initialise "rainbow.shaders" namespace
        lua_pushliteral(L, "shaders");
        lua_createtable(L, 0, 1);
        luaR_rawsetcfunction(L, "diffuse", &diffuse);
        lua_rawset(L, -3);

        reg<Diffuse>(L);
    }
}}}  // rainbow::lua::shaders

auto rainbow::lua::shaders::Diffuse::set_cutoff(lua_State* L) -> int
{
    // <diffuse>:set_cutoff(cutoff)
    checkargs<Diffuse, lua_Number>(L);

    return with_self(L, [](Diffuse* self, lua_State* L) {
        self->lighting_.set_cutoff(lua_tonumber(L, 2));
        return 0;
    });
}

auto rainbow::lua::shaders::Diffuse::set_radius(lua_State* L) -> int
{
    // <diffuse>:set_radius(radius)
    checkargs<Diffuse, lua_Number>(L);

    return with_self(L, [](Diffuse* self, lua_State* L) {
        self->lighting_.set_radius(lua_tonumber(L, 2));
        return 0;
    });
}

auto rainbow::lua::shaders::Diffuse::set_position(lua_State* L) -> int
{
    // <diffuse>:set_position(x, y, z = 100.0)
    checkargs<Diffuse, lua_Number, lua_Number, nil_or<lua_Number>>(L);

    return with_self(L, [](Diffuse* self, lua_State* L) {
        self->lighting_.set_position(lua_tonumber(L, 2),
                                     lua_tonumber(L, 3),
                                     rainbow::lua::optnumber(L, 4, 100.0));
        return 0;
    });
}

LUA_REG_OBJECT(rainbow::lua::shaders::Diffuse, "shaders.diffuse") {
    LUA_REG_OBJECT_FUNC(rainbow::lua::shaders::Diffuse, set_cutoff),
    LUA_REG_OBJECT_FUNC(rainbow::lua::shaders::Diffuse, set_radius),
    LUA_REG_OBJECT_FUNC(rainbow::lua::shaders::Diffuse, set_position),
    LUA_REG_OBJECT_END()
};
