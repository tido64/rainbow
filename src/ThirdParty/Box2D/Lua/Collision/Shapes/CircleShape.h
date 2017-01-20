// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_CIRCLESHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_CIRCLESHAPE_H_

#include <memory>

#include "ThirdParty/Box2D/Lua/Collision/Shapes/Shape.h"

class b2CircleShape;

NS_B2_LUA_BEGIN
{
    class CircleShape : public ShapeBase<CircleShape>
    {
    public:
        static constexpr bool is_constructible = true;
        static const char class_name[];
        static const luaL_Reg functions[];

        explicit CircleShape(lua_State*);
        ~CircleShape();

        b2CircleShape* get() const { return circle_.get(); }

    private:
        static int GetType(lua_State*);
        static int GetRadius(lua_State*);
        static int SetRadius(lua_State*);
        static int GetChildCount(lua_State*);

        std::unique_ptr<b2CircleShape> circle_;
        bool is_owner_;
    };
} NS_B2_LUA_END

#endif
