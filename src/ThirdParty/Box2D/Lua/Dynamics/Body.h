// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_BODY_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_BODY_H_

#include "ThirdParty/Box2D/Lua/Helper.h"

class b2Body;
struct b2BodyDef;

namespace b2 { namespace lua
{
    auto BodyDef(lua_State* L) -> int;
    auto GetBodyDef(lua_State* L) -> b2BodyDef;

    class Body : private rainbow::lua::Object<Body>
    {
    public:
        LUA_REG_OBJECT_PROPS(false)

        static void Init(lua_State*);

        explicit Body(lua_State*);

        auto get() const { return body_; }
        void reset() { body_ = nullptr; }

    private:
        static auto Bind(lua_State*) -> int;

        static auto CreateFixture(lua_State*) -> int;
        static auto DestroyFixture(lua_State*) -> int;

        static auto SetTransform(lua_State*) -> int;
        static auto GetPosition(lua_State*) -> int;
        static auto GetAngle(lua_State*) -> int;
        static auto GetWorldCenter(lua_State*) -> int;
        static auto GetLocalCenter(lua_State*) -> int;

        static auto SetLinearVelocity(lua_State*) -> int;
        static auto GetLinearVelocity(lua_State*) -> int;
        static auto SetAngularVelocity(lua_State*) -> int;
        static auto GetAngularVelocity(lua_State*) -> int;

        static auto ApplyForce(lua_State*) -> int;
        static auto ApplyForceToCenter(lua_State*) -> int;
        static auto ApplyTorque(lua_State*) -> int;
        static auto ApplyLinearImpulse(lua_State*) -> int;
        static auto ApplyAngularImpulse(lua_State*) -> int;

        static auto GetMass(lua_State*) -> int;
        static auto GetInertia(lua_State*) -> int;
        static auto GetMassData(lua_State*) -> int;
        static auto SetMassData(lua_State*) -> int;
        static auto ResetMassData(lua_State*) -> int;

        static auto GetWorldPoint(lua_State*) -> int;
        static auto GetWorldVector(lua_State*) -> int;
        static auto GetLocalPoint(lua_State*) -> int;
        static auto GetLocalVector(lua_State*) -> int;
        static auto GetLinearVelocityFromWorldPoint(lua_State*) -> int;
        static auto GetLinearVelocityFromLocalPoint(lua_State*) -> int;

        static auto GetLinearDamping(lua_State*) -> int;
        static auto SetLinearDamping(lua_State*) -> int;
        static auto GetAngularDamping(lua_State*) -> int;
        static auto SetAngularDamping(lua_State*) -> int;

        static auto GetGravityScale(lua_State*) -> int;
        static auto SetGravityScale(lua_State*) -> int;

        static auto SetBullet(lua_State*) -> int;
        static auto IsBullet(lua_State*) -> int;

        static auto SetSleepingAllowed(lua_State*) -> int;
        static auto IsSleepingAllowed(lua_State*) -> int;
        static auto SetAwake(lua_State*) -> int;
        static auto IsAwake(lua_State*) -> int;
        static auto SetActive(lua_State*) -> int;
        static auto IsActive(lua_State*) -> int;

        static auto SetFixedRotation(lua_State*) -> int;
        static auto IsFixedRotation(lua_State*) -> int;

        static auto GetFixtureList(lua_State*) -> int;
        static auto GetJointList(lua_State*) -> int;
        static auto GetContactList(lua_State*) -> int;
        static auto GetNext(lua_State*) -> int;

        static auto Dump(lua_State*) -> int;

        b2Body* body_;
    };
}}  // namespace b2::lua

#endif
