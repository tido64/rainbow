// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.h"

using b2::lua::PolygonShape;

auto PolygonShape::GetType(lua_State* L) -> int
{
    lua_pushinteger(L, b2Shape::e_polygon);
    return 1;
}

auto PolygonShape::GetChildCount(lua_State* L) -> int
{
    lua_pushinteger(L, 1);
    return 1;
}

auto PolygonShape::Set(lua_State* L) -> int
{
    rainbow::lua::checkargs<PolygonShape, void*, lua_Number>(L);

    return with_self(L, [](PolygonShape* self, lua_State* L) {
        const int count = static_cast<int>(lua_tointeger(L, 3));
        auto points = std::make_unique<b2Vec2[]>(count);
        for (int i = 0; i < count; ++i)
        {
            lua_rawgeti(L, 2, i + 1);
            points[i] = b2Vec2(luaR_getnumber(L, "x"), luaR_getnumber(L, "y"));
            lua_pop(L, 1);
        }
        self->get()->Set(points.get(), count);
        return 0;
    });
}

auto PolygonShape::SetAsBox(lua_State* L) -> int
{
    return with_self(L, [](PolygonShape* self, lua_State* L) {
        const int n = lua_gettop(L);
        LUA_ASSERT(L, n == 3 || n == 5, "3 or 5 parameters expected");

        if (n == 3)
        {
            rainbow::lua::checkargs<PolygonShape, lua_Number, lua_Number>(L);
            self->get()->SetAsBox(lua_tonumber(L, 2), lua_tonumber(L, 3));
        }
        else
        {
            rainbow::lua::checkargs<PolygonShape,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number>(L);

            self->get()->SetAsBox(lua_tonumber(L, 2),
                                  lua_tonumber(L, 3),
                                  Vec2(L, 4, 5),
                                  lua_tonumber(L, 6));
        }

        return 0;
    });
}

auto PolygonShape::Validate(lua_State* L) -> int
{
    return get1b(L, [](const b2PolygonShape* polygon) {
        return polygon->Validate();
    });
}

LUA_REG_OBJECT(PolygonShape, "PolygonShape") {
    LUA_REG_OBJECT_FUNC(PolygonShape, GetType),
    LUA_REG_OBJECT_FUNC(PolygonShape, GetChildCount),
    LUA_REG_OBJECT_FUNC(PolygonShape, Set),
    LUA_REG_OBJECT_FUNC(PolygonShape, SetAsBox),
    LUA_REG_OBJECT_FUNC(PolygonShape, TestPoint),
    LUA_REG_OBJECT_FUNC(PolygonShape, RayCast),
    LUA_REG_OBJECT_FUNC(PolygonShape, ComputeAABB),
    LUA_REG_OBJECT_FUNC(PolygonShape, ComputeMass),
    LUA_REG_OBJECT_FUNC(PolygonShape, Validate),
    LUA_REG_OBJECT_END()
};
