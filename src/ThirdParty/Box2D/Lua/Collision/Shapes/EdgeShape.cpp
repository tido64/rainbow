// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.h"

#include <Box2D/Collision/Shapes/b2EdgeShape.h>

NS_B2_LUA_BEGIN
{
    EdgeShape::EdgeShape(lua_State* L) : is_owner_(false)
    {
        if (lua_isuserdata(L, -1))
            edge_.reset(static_cast<b2EdgeShape*>(lua_touserdata(L, -1)));
        else
        {
            edge_ = std::make_unique<b2EdgeShape>();
            is_owner_ = true;
        }
    }

    EdgeShape::~EdgeShape()
    {
        if (!is_owner_)
            edge_.release();
    }

    int EdgeShape::GetType(lua_State* L)
    {
        lua_pushinteger(L, b2Shape::e_edge);
        return 1;
    }

    int EdgeShape::Set(lua_State* L)
    {
        rainbow::lua::Argument<lua_Number>::is_required(L, 2);
        rainbow::lua::Argument<lua_Number>::is_required(L, 3);
        rainbow::lua::Argument<lua_Number>::is_required(L, 4);
        rainbow::lua::Argument<lua_Number>::is_required(L, 5);

        EdgeShape* self = Bind::self(L);
        if (!self)
            return 0;

        self->get()->Set(Vec2(L, 2, 3), Vec2(L, 4, 5));
        return 0;
    }

    int EdgeShape::GetChildCount(lua_State* L)
    {
        lua_pushinteger(L, 1);
        return 1;
    }

    int EdgeShape::TestPoint(lua_State*)
    {
        return 0;
    }
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
    using b2::lua::EdgeShape;

    template <>
    const char EdgeShape::Bind::class_name[] = "EdgeShape";

    template <>
    const bool EdgeShape::Bind::is_constructible = true;

    template <>
    const luaL_Reg EdgeShape::Bind::functions[]{
        {"GetType",        &EdgeShape::GetType},
        {"Set",            &EdgeShape::Set},
        {"GetChildCount",  &EdgeShape::GetChildCount},
        {"TestPoint",      &EdgeShape::TestPoint},
        {"RayCast",        &EdgeShape::RayCast},
        {"ComputeAABB",    &EdgeShape::ComputeAABB},
        {"ComputeMass",    &EdgeShape::ComputeMass},
        {nullptr,          nullptr}
    };
} NS_RAINBOW_LUA_END
