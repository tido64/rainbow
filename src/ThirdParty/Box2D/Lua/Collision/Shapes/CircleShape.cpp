// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/CircleShape.h"

using b2::lua::CircleShape;

auto CircleShape::GetType(lua_State* L) -> int
{
    lua_pushinteger(L, b2Shape::e_circle);
    return 1;
}

auto CircleShape::GetRadius(lua_State* L) -> int
{
    return get1f(L, [](const b2CircleShape* circle) {
        return circle->m_radius;
    });
}

auto CircleShape::SetRadius(lua_State* L) -> int
{
    return set1f(L, [](b2CircleShape* circle, float r) {
        circle->m_radius = r;
    });
}

auto CircleShape::GetChildCount(lua_State* L) -> int
{
    lua_pushinteger(L, 1);
    return 1;
}

LUA_REG_OBJECT(CircleShape, "CircleShape") {
    LUA_REG_OBJECT_FUNC(CircleShape, GetType),
    LUA_REG_OBJECT_FUNC(CircleShape, GetRadius),
    LUA_REG_OBJECT_FUNC(CircleShape, SetRadius),
    LUA_REG_OBJECT_FUNC(CircleShape, GetChildCount),
    LUA_REG_OBJECT_FUNC(CircleShape, TestPoint),
    LUA_REG_OBJECT_FUNC(CircleShape, RayCast),
    LUA_REG_OBJECT_FUNC(CircleShape, ComputeAABB),
    LUA_REG_OBJECT_FUNC(CircleShape, ComputeMass),
    LUA_REG_OBJECT_END()
};
