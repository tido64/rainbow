// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Dynamics/Fixture.h"

#include <Box2D/Dynamics/b2Fixture.h>

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"
#include "ThirdParty/Box2D/Lua/Dynamics/Body.h"
#include "ThirdParty/Box2D/StableWorld.h"

using b2::lua::Fixture;

auto b2::lua::Filter(lua_State* L) -> int
{
    const b2Filter filter;
    return Filter(L, filter);
}

auto b2::lua::Filter(lua_State* L, const b2Filter& filter) -> int
{
    lua_createtable(L, 0, 3);
    luaR_rawsetinteger(L, "categoryBits", filter.categoryBits);
    luaR_rawsetinteger(L, "maskBits", filter.maskBits);
    luaR_rawsetinteger(L, "groupIndex", filter.groupIndex);
    return 1;
}

auto b2::lua::GetFilter(lua_State* L) -> b2Filter
{
    b2Filter filter;
    filter.categoryBits = luaR_getinteger(L, "categoryBits");
    filter.maskBits = luaR_getinteger(L, "maskBits");
    filter.groupIndex = luaR_getinteger(L, "groupIndex");
    return filter;
}

auto b2::lua::FixtureDef(lua_State* L) -> int
{
    lua_createtable(L, 0, 6);

    luaR_rawsetnil(L, "shape");
    luaR_rawsetnumber(L, "friction", 0.2f);
    luaR_rawsetnumber(L, "restitution", 0.0f);
    luaR_rawsetnumber(L, "density", 0.0f);
    luaR_rawsetboolean(L, "isSensor", false);

    lua_pushliteral(L, "filter");
    Filter(L);
    lua_rawset(L, -3);

    return 1;
}

auto b2::lua::GetFixtureDef(lua_State* L) -> b2FixtureDef
{
    b2FixtureDef fd;

    luaR_getfield(L, "shape");
    fd.shape = GetShape(L);
    lua_pop(L, 1);

    fd.friction = luaR_getnumber(L, "friction");
    fd.restitution = luaR_getnumber(L, "restitution");
    fd.density = luaR_getnumber(L, "density");
    fd.isSensor = luaR_getboolean(L, "isSensor");

    luaR_getfield(L, "filter");
    fd.filter = GetFilter(L);
    lua_pop(L, 1);

    return fd;
}

void Fixture::Init(lua_State* L)
{
    luaR_rawsetcfunction(L, "Filter", &Filter);
    luaR_rawsetcfunction(L, "FixtureDef", &FixtureDef);

    rainbow::lua::reg<Fixture>(L);
}

Fixture::Fixture(lua_State* L)
    : fixture_(static_cast<b2Fixture*>(lua_touserdata(L, -1)))
{
}

auto Fixture::GetType(lua_State* L) -> int
{
    return get1i(L, [](const b2Fixture* fixture) {
        return fixture->GetType();
    });
}

auto Fixture::GetShape(lua_State* L) -> int
{
    return with_self(L, [](Fixture* self, lua_State* L) {
        return Shape(L, self->get()->GetShape());
    });
}

auto Fixture::SetSensor(lua_State* L) -> int
{
    return set1b(L, [](b2Fixture* fixture, bool sensor) {
        fixture->SetSensor(sensor);
    });
}

auto Fixture::IsSensor(lua_State* L) -> int
{
    return get1b(L, [](const b2Fixture* fixture) {
        return fixture->IsSensor();
    });
}

auto Fixture::SetFilterData(lua_State* L) -> int
{
    rainbow::lua::checkargs<Fixture, void*>(L);

    return with_self(L, [](Fixture* self, lua_State* L) {
        self->get()->SetFilterData(GetFilter(L));
        return 0;
    });
}

auto Fixture::GetFilterData(lua_State* L) -> int
{
    return with_self(L, [](Fixture* self, lua_State* L) {
        const b2Filter& filter = self->get()->GetFilterData();
        return Filter(L, filter);
    });
}

auto Fixture::Refilter(lua_State* L) -> int
{
    return with_self(L, [](Fixture* self, lua_State*) {
        self->get()->Refilter();
        return 0;
    });
}

auto Fixture::GetBody(lua_State* L) -> int
{
    return get1ud<Body>(L, [](b2Fixture* fixture) {
        return fixture->GetBody();
    });
}

auto Fixture::GetNext(lua_State* L) -> int
{
    return get1ud<Fixture>(L, [](b2Fixture* fixture) {
        return fixture->GetNext();
    });
}

auto Fixture::TestPoint(lua_State* L) -> int
{
    rainbow::lua::checkargs<Fixture, lua_Number, lua_Number>(L);

    return get1b(
        L,
        [](const b2Fixture* fixture, const b2Vec2& p) {
            return fixture->TestPoint(p);
        },
        Vec2(L, 2, 3));
}

auto Fixture::RayCast(lua_State*) -> int
{
    return -1;
}

auto Fixture::GetMassData(lua_State* L) -> int
{
    return with_self(L, [](Fixture* self, lua_State* L) {
        b2MassData mass;
        self->get()->GetMassData(&mass);
        return MassData(L, mass);
    });
}

auto Fixture::SetDensity(lua_State* L) -> int
{
    return set1f(L, [](b2Fixture* fixture, float density) {
        fixture->SetDensity(density);
    });
}

auto Fixture::GetDensity(lua_State* L) -> int
{
    return get1f(L, [](const b2Fixture* fixture) {
        return fixture->GetDensity();
    });
}

auto Fixture::GetFriction(lua_State* L) -> int
{
    return get1f(L, [](const b2Fixture* fixture) {
        return fixture->GetFriction();
    });
}

auto Fixture::SetFriction(lua_State* L) -> int
{
    return set1f(L, [](b2Fixture* fixture, float friction) {
        fixture->SetFriction(friction);
    });
}

auto Fixture::GetRestitution(lua_State* L) -> int
{
    return get1f(L, [](const b2Fixture* fixture) {
        return fixture->GetRestitution();
    });
}

auto Fixture::SetRestitution(lua_State* L) -> int
{
    return set1f(L, [](b2Fixture* fixture, float restitution) {
        fixture->SetRestitution(restitution);
    });
}

auto Fixture::GetAABB(lua_State*) -> int
{
    return -1;
}

auto Fixture::Dump(lua_State* L) -> int
{
    rainbow::lua::checkargs<Fixture, lua_Number>(L);

    return with_self(L, [](Fixture* self, lua_State* L) {
        self->get()->Dump(static_cast<int32>(lua_tointeger(L, 2)));
        return 0;
    });
}

LUA_REG_OBJECT(Fixture, "b2Fixture") {
    LUA_REG_OBJECT_FUNC(Fixture, GetType),
    LUA_REG_OBJECT_FUNC(Fixture, GetShape),
    LUA_REG_OBJECT_FUNC(Fixture, SetSensor),
    LUA_REG_OBJECT_FUNC(Fixture, IsSensor),
    LUA_REG_OBJECT_FUNC(Fixture, SetFilterData),
    LUA_REG_OBJECT_FUNC(Fixture, GetFilterData),
    LUA_REG_OBJECT_FUNC(Fixture, Refilter),
    LUA_REG_OBJECT_FUNC(Fixture, GetBody),
    LUA_REG_OBJECT_FUNC(Fixture, GetNext),
    LUA_REG_OBJECT_FUNC(Fixture, TestPoint),
    LUA_REG_OBJECT_FUNC(Fixture, RayCast),
    LUA_REG_OBJECT_FUNC(Fixture, GetMassData),
    LUA_REG_OBJECT_FUNC(Fixture, SetDensity),
    LUA_REG_OBJECT_FUNC(Fixture, GetDensity),
    LUA_REG_OBJECT_FUNC(Fixture, GetFriction),
    LUA_REG_OBJECT_FUNC(Fixture, SetFriction),
    LUA_REG_OBJECT_FUNC(Fixture, GetRestitution),
    LUA_REG_OBJECT_FUNC(Fixture, SetRestitution),
    LUA_REG_OBJECT_FUNC(Fixture, GetAABB),
    LUA_REG_OBJECT_FUNC(Fixture, Dump),
    LUA_REG_OBJECT_END()
};
