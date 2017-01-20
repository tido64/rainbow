// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

#include <Box2D/Collision/Shapes/b2ChainShape.h>
#include <Box2D/Collision/Shapes/b2CircleShape.h>
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#include <Box2D/Collision/Shapes/b2PolygonShape.h>

//#include "ThirdParty/Box2D/Lua/Collision/Shapes/ChainShape.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/CircleShape.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/EdgeShape.h"
#include "ThirdParty/Box2D/Lua/Collision/Shapes/PolygonShape.h"

namespace
{
    const char* const kShapeTypes[]{
        b2::lua::CircleShape::class_name,
        b2::lua::EdgeShape::class_name,
        b2::lua::PolygonShape::class_name,
        //b2::lua::ChainShape::class_name
    };
}

NS_B2_LUA_BEGIN
{
    int ShapesInit(lua_State* L)
    {
        lua_pushliteral(L, "Shape");
        lua_createtable(L, 5, 0);
        luaR_rawsetinteger(L, "e_circle", b2Shape::e_circle);
        luaR_rawsetinteger(L, "e_edge", b2Shape::e_edge);
        luaR_rawsetinteger(L, "e_polygon", b2Shape::e_polygon);
        luaR_rawsetinteger(L, "e_chain", b2Shape::e_chain);
        luaR_rawsetinteger(L, "e_typeCount", b2Shape::e_typeCount);
        lua_rawset(L, -3);

        //rainbow::lua::reg<ChainShape>(L);
        rainbow::lua::reg<CircleShape>(L);
        rainbow::lua::reg<EdgeShape>(L);
        rainbow::lua::reg<PolygonShape>(L);
        return 0;
    }

    int MassData(lua_State* L, const b2MassData& mass)
    {
        lua_pushnumber(L, mass.mass);
        lua_pushnumber(L, mass.center.x);
        lua_pushnumber(L, mass.center.y);
        lua_pushnumber(L, mass.I);
        return 4;
    }

    int Shape(lua_State* L, b2Shape* shape)
    {
        lua_pushlightuserdata(L, shape);
        switch (shape->GetType())
        {
            case b2Shape::e_circle:
                return rainbow::lua::alloc<CircleShape>(L);
            case b2Shape::e_edge:
                return rainbow::lua::alloc<EdgeShape>(L);
            case b2Shape::e_polygon:
                return rainbow::lua::alloc<PolygonShape>(L);
            case b2Shape::e_chain:
                //return rainbow::lua::alloc<ChainShape>(L);
            default:
                break;
        }
        return 0;
    }

    b2Shape* GetShape(lua_State* L)
    {
        for (int i = 0; i < b2Shape::e_typeCount; ++i)
        {
            void* ud = luaL_testudata(L, -1, kShapeTypes[i]);
            if (ud != nullptr)
            {
                switch (i)
                {
                    case b2Shape::e_circle:
                        return static_cast<CircleShape*>(ud)->get();
                    case b2Shape::e_edge:
                        return static_cast<EdgeShape*>(ud)->get();
                    case b2Shape::e_polygon:
                        return static_cast<PolygonShape*>(ud)->get();
                    case b2Shape::e_chain:
                        //return static_cast<ChainShape*>(ud)->get();
                    default:
                        break;
                }
                break;
            }
        }
        return nullptr;
    }
} NS_B2_LUA_END
