// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.h"

#include <memory>

#include <Box2D/Collision/Shapes/b2PolygonShape.h>

NS_B2_LUA_BEGIN
{
    PolygonShape::PolygonShape(lua_State* L)
        : polygon_(nullptr), is_owner_(false)
    {
        if (lua_isuserdata(L, -1))
            polygon_ = static_cast<b2PolygonShape*>(lua_touserdata(L, -1));
        else
        {
            polygon_ = new b2PolygonShape();
            is_owner_ = true;
        }
    }

    PolygonShape::~PolygonShape()
    {
        if (is_owner_)
            delete polygon_;
    }

    int PolygonShape::GetType(lua_State* L)
    {
        lua_pushinteger(L, b2Shape::e_polygon);
        return 1;
    }

    int PolygonShape::GetChildCount(lua_State* L)
    {
        lua_pushinteger(L, 1);
        return 1;
    }

    int PolygonShape::Set(lua_State* L)
    {
        rainbow::lua::Argument<void*>::is_required(L, 2);
        rainbow::lua::Argument<lua_Number>::is_required(L, 3);

        PolygonShape* self = Bind::self(L);
        if (!self)
            return 0;

        const int count = static_cast<int>(lua_tointeger(L, 3));
        std::unique_ptr<b2Vec2[]> points(new b2Vec2[count]);
        for (int i = 0; i < count; ++i)
        {
            lua_rawgeti(L, 2, i + 1);
            points[i] = b2Vec2(luaR_getnumber(L, "x"), luaR_getnumber(L, "y"));
            lua_pop(L, 1);
        }
        self->get()->Set(points.get(), count);
        return 0;
    }

    int PolygonShape::SetAsBox(lua_State* L)
    {
        rainbow::lua::Argument<lua_Number>::is_required(L, 2);
        rainbow::lua::Argument<lua_Number>::is_required(L, 3);

        PolygonShape* self = Bind::self(L);
        if (!self)
            return 0;

        const int n = lua_gettop(L);
        LUA_ASSERT(L, n == 3 || n == 5, "3 or 5 parameters expected");

        if (n == 3)
            self->get()->SetAsBox(lua_tonumber(L, 2), lua_tonumber(L, 3));
        else
        {
            rainbow::lua::Argument<lua_Number>::is_required(L, 4);
            rainbow::lua::Argument<lua_Number>::is_required(L, 5);
            rainbow::lua::Argument<lua_Number>::is_required(L, 6);

            self->get()->SetAsBox(lua_tonumber(L, 2),
                                  lua_tonumber(L, 3),
                                  Vec2(L, 4, 5),
                                  lua_tonumber(L, 6));
        }
        return 0;
    }

    int PolygonShape::Validate(lua_State* L)
    {
        return get1b(L, [](const b2PolygonShape* polygon) {
            return polygon->Validate();
        });
    }
} NS_B2_LUA_END

NS_RAINBOW_LUA_BEGIN
{
    using b2::lua::PolygonShape;

    template <>
    const char PolygonShape::Bind::class_name[] = "PolygonShape";

    template <>
    const bool PolygonShape::Bind::is_constructible = true;

    template <>
    const luaL_Reg PolygonShape::Bind::functions[]{
        {"GetType",         &PolygonShape::GetType},
        {"GetChildCount",   &PolygonShape::GetChildCount},
        {"Set",             &PolygonShape::Set},
        {"SetAsBox",        &PolygonShape::SetAsBox},
        {"TestPoint",       &PolygonShape::TestPoint},
        {"RayCast",         &PolygonShape::RayCast},
        {"ComputeAABB",     &PolygonShape::ComputeAABB},
        {"ComputeMass",     &PolygonShape::ComputeMass},
        {"Validate",        &PolygonShape::Validate},
        {nullptr,           nullptr}
    };
} NS_RAINBOW_LUA_END
