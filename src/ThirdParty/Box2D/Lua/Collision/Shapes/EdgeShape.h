// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_EDGESHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_EDGESHAPE_H_

#ifdef __GNUC__
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif
#include <Box2D/Collision/Shapes/b2EdgeShape.h>
#ifdef __GNUC__
#   pragma GCC diagnostic pop
#endif

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

class b2EdgeShape;

namespace b2 { namespace lua
{
    class EdgeShape : public ShapeBase<EdgeShape, b2EdgeShape>
    {
    public:
        LUA_REG_OBJECT_PROPS(true)

        explicit EdgeShape(lua_State* L) : ShapeBase(L) {}

    private:
        static auto GetType(lua_State*) -> int;
        static auto Set(lua_State*) -> int;
        static auto GetChildCount(lua_State*) -> int;
        static auto TestPoint(lua_State*) -> int /* override */;
    };
}}  // namespace b2::lua

#endif
