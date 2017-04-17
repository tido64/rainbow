// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_FIXTURE_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_FIXTURE_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

struct b2Filter;
class b2Fixture;
struct b2FixtureDef;

namespace b2 { namespace lua
{
    auto Filter(lua_State*) -> int;
    auto Filter(lua_State*, const b2Filter& filter) -> int;
    auto GetFilter(lua_State*) -> b2Filter;

    auto FixtureDef(lua_State*) -> int;
    auto GetFixtureDef(lua_State*) -> b2FixtureDef;

    class Fixture : private rainbow::lua::Object<Fixture>
    {
    public:
        LUA_REG_OBJECT_PROPS(false)

        static void Init(lua_State*);

        explicit Fixture(lua_State*);

        auto get() const { return fixture_; }
        void reset() { fixture_ = nullptr; }

    private:
        static auto GetType(lua_State*) -> int;
        static auto GetShape(lua_State*) -> int;
        static auto SetSensor(lua_State*) -> int;
        static auto IsSensor(lua_State*) -> int;
        static auto SetFilterData(lua_State*) -> int;
        static auto GetFilterData(lua_State*) -> int;
        static auto Refilter(lua_State*) -> int;
        static auto GetBody(lua_State*) -> int;
        static auto GetNext(lua_State*) -> int;
        //static auto GetUserData(lua_State*) -> int;
        //static auto SetUserData(lua_State*) -> int;
        static auto TestPoint(lua_State*) -> int;
        static auto RayCast(lua_State*) -> int;
        static auto GetMassData(lua_State*) -> int;
        static auto SetDensity(lua_State*) -> int;
        static auto GetDensity(lua_State*) -> int;
        static auto GetFriction(lua_State*) -> int;
        static auto SetFriction(lua_State*) -> int;
        static auto GetRestitution(lua_State*) -> int;
        static auto SetRestitution(lua_State*) -> int;
        static auto GetAABB(lua_State*) -> int;
        static auto Dump(lua_State*) -> int;

        b2Fixture* fixture_;
    };
}}  // namespace b2::lua

#endif
