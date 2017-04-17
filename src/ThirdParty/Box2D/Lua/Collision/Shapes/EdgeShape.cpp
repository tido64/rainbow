// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.h"

using b2::lua::EdgeShape;

auto EdgeShape::GetType(lua_State* L) -> int
{
    lua_pushinteger(L, b2Shape::e_edge);
    return 1;
}

auto EdgeShape::Set(lua_State* L) -> int
{
    rainbow::lua::checkargs<EdgeShape,
                            lua_Number,
                            lua_Number,
                            lua_Number,
                            lua_Number>(L);

    return with_self(L, [](EdgeShape* self, lua_State* L) {
        self->get()->Set(Vec2(L, 2, 3), Vec2(L, 4, 5));
        return 0;
    });
}

auto EdgeShape::GetChildCount(lua_State* L) -> int
{
    lua_pushinteger(L, 1);
    return 1;
}

auto EdgeShape::TestPoint(lua_State*) -> int
{
    return 0;
}

LUA_REG_OBJECT(EdgeShape, "EdgeShape") {
    LUA_REG_OBJECT_FUNC(EdgeShape, GetType),
    LUA_REG_OBJECT_FUNC(EdgeShape, Set),
    LUA_REG_OBJECT_FUNC(EdgeShape, GetChildCount),
    LUA_REG_OBJECT_FUNC(EdgeShape, TestPoint),
    LUA_REG_OBJECT_FUNC(EdgeShape, RayCast),
    LUA_REG_OBJECT_FUNC(EdgeShape, ComputeAABB),
    LUA_REG_OBJECT_FUNC(EdgeShape, ComputeMass),
    LUA_REG_OBJECT_END()
};
