// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_SHAPE_H_
#define THIRDPARTY_BOX2D_LUA_COLLISION_SHAPES_SHAPE_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

struct b2MassData;
class b2Shape;

namespace b2 { namespace lua
{
    auto ShapesInit(lua_State*) -> int;

    auto MassData(lua_State*, const b2MassData&) -> int;

    auto Shape(lua_State*, b2Shape*) -> int;
    auto GetShape(lua_State*) -> b2Shape*;

    template <typename T, typename U>
    class ShapeBase : protected rainbow::lua::Object<T>
    {
    public:
        auto get() const { return shape_.get(); }

    protected:
        static auto TestPoint(lua_State* L) -> int
        {
            rainbow::lua::checkargs<T,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number,
                                    lua_Number>(L);

            return T::with_self(L, [](T* self, lua_State* L) {
                const b2Transform t(Vec2(L, 2, 3), b2Rot(lua_tonumber(L, 4)));
                rainbow::lua::push(L, self->get()->TestPoint(t, Vec2(L, 5, 6)));
                return 1;
            });
        }

        static auto RayCast(lua_State*) -> int { return -1; }
        static auto ComputeAABB(lua_State*) -> int { return -1; }
        static auto ComputeMass(lua_State*) -> int { return -1; }

        explicit ShapeBase(lua_State* L)
            : is_owner_(!rainbow::lua::isuserdata(L, -1))
        {
            if (!is_owner_)
                shape_.reset(static_cast<U*>(lua_touserdata(L, -1)));
            else
                shape_ = std::make_unique<U>();
        }

        ~ShapeBase()
        {
            if (!is_owner_)
                shape_.release();
        }

    private:
        std::unique_ptr<U> shape_;
        const bool is_owner_;
    };
}}  // namespace b2::lua

#endif
