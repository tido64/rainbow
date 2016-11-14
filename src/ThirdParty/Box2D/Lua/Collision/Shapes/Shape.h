// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_SHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_SHAPE_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

struct b2MassData;
class b2Shape;

NS_B2_LUA_BEGIN
{
    int ShapesInit(lua_State* L);

    int MassData(lua_State* L, const b2MassData& mass);

    int Shape(lua_State* L, b2Shape* shape);
    b2Shape* GetShape(lua_State* L);

    template <typename T>
    class ShapeBase : protected rainbow::lua::Bind<T>
    {
    protected:
        static int TestPoint(lua_State* L)
        {
            rainbow::lua::checkargs<T,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number>(L);

            T* self = T::self(L);
            if (!self)
                return 0;

            const b2Transform t(Vec2(L, 2, 3), b2Rot(lua_tonumber(L, 4)));
            rainbow::lua::push(L, self->get()->TestPoint(t, Vec2(L, 5, 6)));
            return 1;
        }

        static int RayCast(lua_State*) { return -1; }
        static int ComputeAABB(lua_State*) { return -1; }
        static int ComputeMass(lua_State*) { return -1; }

        ShapeBase() = default;
        ~ShapeBase() = default;
    };
} NS_B2_LUA_END

#endif
