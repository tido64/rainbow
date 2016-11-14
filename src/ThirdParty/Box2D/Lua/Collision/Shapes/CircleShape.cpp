// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/CircleShape.h"

#include <Box2D/Collision/Shapes/b2CircleShape.h>

NS_B2_LUA_BEGIN
{
    constexpr bool CircleShape::is_constructible;

    const char CircleShape::class_name[] = "CircleShape";

    const luaL_Reg CircleShape::functions[]{
        {"GetType",        &CircleShape::GetType},
        {"GetRadius",      &CircleShape::GetRadius},
        {"SetRadius",      &CircleShape::SetRadius},
        {"GetChildCount",  &CircleShape::GetChildCount},
        {"TestPoint",      &CircleShape::TestPoint},
        {"RayCast",        &CircleShape::RayCast},
        {"ComputeAABB",    &CircleShape::ComputeAABB},
        {"ComputeMass",    &CircleShape::ComputeMass},
        {nullptr,          nullptr}};

    CircleShape::CircleShape(lua_State* L) : is_owner_(false)
    {
        if (rainbow::lua::isuserdata(L, -1))
            circle_.reset(static_cast<b2CircleShape*>(lua_touserdata(L, -1)));
        else
        {
            circle_ = std::make_unique<b2CircleShape>();
            is_owner_ = true;
        }
    }

    CircleShape::~CircleShape()
    {
        if (!is_owner_)
            circle_.release();
    }

    int CircleShape::GetType(lua_State* L)
    {
        lua_pushinteger(L, b2Shape::e_circle);
        return 1;
    }

    int CircleShape::GetRadius(lua_State* L)
    {
        return get1f(L, [](const b2CircleShape* circle) {
            return circle->m_radius;
        });
    }

    int CircleShape::SetRadius(lua_State* L)
    {
        return set1f(L, [](b2CircleShape* circle, float r) {
            circle->m_radius = r;
        });
    }

    int CircleShape::GetChildCount(lua_State* L)
    {
        lua_pushinteger(L, 1);
        return 1;
    }
} NS_B2_LUA_END
