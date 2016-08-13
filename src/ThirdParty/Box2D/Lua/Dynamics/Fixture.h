// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_FIXTURE_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_FIXTURE_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

struct b2Filter;
class b2Fixture;
struct b2FixtureDef;

NS_B2_LUA_BEGIN
{
    int Filter(lua_State* L);
    int Filter(lua_State* L, const b2Filter& filter);
    b2Filter GetFilter(lua_State* L);

    int FixtureDef(lua_State* L);
    b2FixtureDef GetFixtureDef(lua_State* L);

    class Fixture : private rainbow::lua::Bind<Fixture>
    {
    public:
        static constexpr bool is_constructible = false;
        static const char class_name[];
        static const luaL_Reg functions[];

        static void Init(lua_State*);

        explicit Fixture(lua_State*);

        b2Fixture* get() const { return fixture_; }
        void reset() { fixture_ = nullptr; }

    private:
        static int GetType(lua_State*);
        static int GetShape(lua_State*);
        static int SetSensor(lua_State*);
        static int IsSensor(lua_State*);
        static int SetFilterData(lua_State*);
        static int GetFilterData(lua_State*);
        static int Refilter(lua_State*);
        static int GetBody(lua_State*);
        static int GetNext(lua_State*);
        //static int GetUserData(lua_State*);
        //static int SetUserData(lua_State*);
        static int TestPoint(lua_State*);
        static int RayCast(lua_State*);
        static int GetMassData(lua_State*);
        static int SetDensity(lua_State*);
        static int GetDensity(lua_State*);
        static int GetFriction(lua_State*);
        static int SetFriction(lua_State*);
        static int GetRestitution(lua_State*);
        static int SetRestitution(lua_State*);
        static int GetAABB(lua_State*);
        static int Dump(lua_State*);

        b2Fixture* fixture_;
    };
} NS_B2_LUA_END

#endif
