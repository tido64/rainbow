// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_LUA_DYNAMICS_WORLD_H_
#define THIRDPARTY_BOX2D_LUA_DYNAMICS_WORLD_H_

#include <memory>

#include <Box2D/Dynamics/b2WorldCallbacks.h>

#include "Lua/Object.h"

namespace b2
{
    class StableWorld;

    namespace lua
    {
        class World : public b2ContactListener,
                      private rainbow::lua::Object<World>
        {
        public:
            LUA_REG_OBJECT_PROPS(true)

            explicit World(lua_State*);
            ~World();

            auto get() const { return world_.get(); }

            // Implement b2ContactListener

            void BeginContact(b2Contact*) override;
            void EndContact(b2Contact*) override;
            void PreSolve(b2Contact*, const b2Manifold*) override;
            void PostSolve(b2Contact*, const b2ContactImpulse*) override;

        private:
            static auto GetPTM(lua_State*) -> int;
            static auto SetPTM(lua_State*) -> int;

            static auto SetDestructionListener(lua_State*) -> int;
            static auto SetContactFilter(lua_State*) -> int;
            static auto SetContactListener(lua_State*) -> int;
            static auto SetDebugDraw(lua_State*) -> int;
            static auto CreateBody(lua_State*) -> int;
            static auto DestroyBody(lua_State*) -> int;
            static auto CreateJoint(lua_State*) -> int;
            static auto DestroyJoint(lua_State*) -> int;
            static auto Step(lua_State*) -> int;
            static auto QueryAABB(lua_State*) -> int;
            static auto RayCast(lua_State*) -> int;
            static auto GetBodyList(lua_State*) -> int;
            static auto GetJointList(lua_State*) -> int;
            static auto GetContactList(lua_State*) -> int;
            static auto SetAllowSleeping(lua_State*) -> int;
            static auto GetAllowSleeping(lua_State*) -> int;
            static auto SetWarmStarting(lua_State*) -> int;
            static auto GetWarmStarting(lua_State*) -> int;
            static auto SetContinuousPhysics(lua_State*) -> int;
            static auto GetContinuousPhysics(lua_State*) -> int;
            static auto SetSubStepping(lua_State*) -> int;
            static auto GetSubStepping(lua_State*) -> int;
            static auto GetProxyCount(lua_State*) -> int;
            static auto GetBodyCount(lua_State*) -> int;
            static auto GetJointCount(lua_State*) -> int;
            static auto GetContactCount(lua_State*) -> int;
            static auto GetTreeHeight(lua_State*) -> int;
            static auto GetTreeBalance(lua_State*) -> int;
            static auto GetTreeQuality(lua_State*) -> int;
            static auto SetGravity(lua_State*) -> int;
            static auto GetGravity(lua_State*) -> int;
            static auto IsLocked(lua_State*) -> int;
            static auto ShiftOrigin(lua_State*) -> int;
            static auto GetContactManager(lua_State*) -> int;
            static auto GetProfile(lua_State*) -> int;
            static auto Dump(lua_State*) -> int;

            std::unique_ptr<StableWorld> world_;
            int contact_listener_;
            lua_State* state_;
        };
    }
}

#endif
