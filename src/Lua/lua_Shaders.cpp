// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Lua/lua_Shaders.h"

#include "Graphics/Shaders/Diffuse.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_MODULE_BEGIN(shaders)
{
    class Diffuse final : public rainbow::lua::Shader,
                          private rainbow::lua::Bind<Diffuse>
    {
    public:
        static constexpr bool is_constructible = false;
        static const char class_name[];
        static const luaL_Reg functions[];

        explicit Diffuse(lua_State* L) : lighting_(toboolean(L, 1)) {}

        const rainbow::shaders::Diffuse* get() const { return &lighting_; }

        int id() const override { return lighting_.id(); }

    private:
        static int set_cutoff(lua_State*);
        static int set_radius(lua_State*);
        static int set_position(lua_State*);

        rainbow::shaders::Diffuse lighting_;
    };

    int diffuse(lua_State* L) { return rainbow::lua::alloc<Diffuse>(L); }

    void init(lua_State* L)
    {
        // Initialise "rainbow.shaders" namespace
        lua_pushliteral(L, "shaders");
        lua_createtable(L, 0, 1);
        luaR_rawsetcfunction(L, "diffuse", &diffuse);
        lua_rawset(L, -3);

        reg<Diffuse>(L);
    }
} NS_RAINBOW_LUA_MODULE_END(shaders)

constexpr bool rainbow::lua::shaders::Diffuse::is_constructible;

const char rainbow::lua::shaders::Diffuse::class_name[] = "shaders.diffuse";

const luaL_Reg rainbow::lua::shaders::Diffuse::functions[]{
    {"set_cutoff",    &rainbow::lua::shaders::Diffuse::set_cutoff},
    {"set_radius",    &rainbow::lua::shaders::Diffuse::set_radius},
    {"set_position",  &rainbow::lua::shaders::Diffuse::set_position},
    {nullptr,         nullptr}};

int rainbow::lua::shaders::Diffuse::set_cutoff(lua_State* L)
{
    // <diffuse>:set_cutoff(cutoff)
    checkargs<Diffuse, lua_Number>(L);

    Diffuse* self = Bind::self(L);
    if (self == nullptr)
        return 0;

    self->lighting_.set_cutoff(lua_tonumber(L, 2));
    return 0;
}

int rainbow::lua::shaders::Diffuse::set_radius(lua_State* L)
{
    // <diffuse>:set_radius(radius)
    checkargs<Diffuse, lua_Number>(L);

    Diffuse* self = Bind::self(L);
    if (self == nullptr)
        return 0;

    self->lighting_.set_radius(lua_tonumber(L, 2));
    return 0;
}

int rainbow::lua::shaders::Diffuse::set_position(lua_State* L)
{
    // <diffuse>:set_position(x, y, z = 100.0)
    checkargs<Diffuse, lua_Number, lua_Number, nil_or<lua_Number>>(L);

    Diffuse* self = Bind::self(L);
    if (self == nullptr)
        return 0;

    self->lighting_.set_position(lua_tonumber(L, 2),
                                 lua_tonumber(L, 3),
                                 rainbow::lua::optnumber(L, 4, 100.0));
    return 0;
}
