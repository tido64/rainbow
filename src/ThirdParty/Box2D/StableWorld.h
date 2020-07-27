// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_STABLEWORLD_H_
#define THIRDPARTY_BOX2D_STABLEWORLD_H_

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <box2d/b2_world.h>  // NOLINT(llvm-include-order)
#include "ThirdParty/ReenableWarnings.h"
// clang-format on

#include "Graphics/Sprite.h"
#include "ThirdParty/Box2D/DebugDraw.h"

namespace b2
{
    struct BodyState
    {
        rainbow::SpriteRef sprite;
        b2Vec2 curr_p;
        b2Vec2 prev_p;
        float curr_r;
        float prev_r;

        BodyState(const b2BodyDef* d);
    };

    class StableWorld final : public DebuggableWorld
    {
    public:
        StableWorld(float gx = 0.0f, float gy = rainbow::kStandardGravity);
        ~StableWorld();

        void SetDebugDraw(b2Draw* debugDraw);
        auto CreateBody(const b2BodyDef* def) -> b2Body*;
        void DestroyBody(b2Body* body);
        void Step(float timeStep,
                  int32 velocityIterations,
                  int32 positionIterations);

        // b2World forwards

        void SetDestructionListener(b2DestructionListener* listener)
        {
            world_.SetDestructionListener(listener);
        }

        void SetContactFilter(b2ContactFilter* filter)
        {
            world_.SetContactFilter(filter);
        }

        void SetContactListener(b2ContactListener* listener)
        {
            world_.SetContactListener(listener);
        }

        auto CreateJoint(const b2JointDef* def)
        {
            return world_.CreateJoint(def);
        }

        void DestroyJoint(b2Joint* joint) { world_.DestroyJoint(joint); }

        void ClearForces() { world_.ClearForces(); }

        void QueryAABB(b2QueryCallback* callback, const b2AABB& aabb) const
        {
            world_.QueryAABB(callback, aabb);
        }

        void RayCast(b2RayCastCallback* callback,
                     const b2Vec2& point1,
                     const b2Vec2& point2) const
        {
            world_.RayCast(callback, point1, point2);
        }

        auto GetBodyList() { return world_.GetBodyList(); }
        auto GetBodyList() const { return world_.GetBodyList(); }

        auto GetJointList() { return world_.GetJointList(); }
        auto GetJointList() const { return world_.GetJointList(); }

        auto GetContactList() { return world_.GetContactList(); }
        auto GetContactList() const { return world_.GetContactList(); }

        void SetAllowSleeping(bool flag) { world_.SetAllowSleeping(flag); }
        auto GetAllowSleeping() const { return world_.GetAllowSleeping(); }

        void SetWarmStarting(bool flag) { world_.SetWarmStarting(flag); }
        auto GetWarmStarting() const { return world_.GetWarmStarting(); }

        void SetContinuousPhysics(bool flag)
        {
            world_.SetContinuousPhysics(flag);
        }

        auto GetContinuousPhysics() const
        {
            return world_.GetContinuousPhysics();
        }

        void SetSubStepping(bool flag) { world_.SetSubStepping(flag); }
        auto GetSubStepping() const { return world_.GetSubStepping(); }

        auto GetProxyCount() const { return world_.GetProxyCount(); }

        auto GetBodyCount() const { return world_.GetBodyCount(); }

        auto GetJointCount() const { return world_.GetJointCount(); }

        auto GetContactCount() const { return world_.GetContactCount(); }

        auto GetTreeHeight() const { return world_.GetTreeHeight(); }

        auto GetTreeBalance() const { return world_.GetTreeBalance(); }

        auto GetTreeQuality() const { return world_.GetTreeQuality(); }

        void SetGravity(const b2Vec2& gravity) { world_.SetGravity(gravity); }

        auto GetGravity() const { return world_.GetGravity(); }

        auto IsLocked() const { return world_.IsLocked(); }

        void SetAutoClearForces(bool flag) { world_.SetAutoClearForces(flag); }

        auto GetAutoClearForces() const { return world_.GetAutoClearForces(); }

        void ShiftOrigin(const b2Vec2& newOrigin)
        {
            world_.ShiftOrigin(newOrigin);
        }

        const auto& GetContactManager() const
        {
            return world_.GetContactManager();
        }

        const auto& GetProfile() const { return world_.GetProfile(); }

        void Dump() { world_.Dump(); }

        // DebuggableWorld implementation details

        void DrawDebugData() override;

    private:
        float elapsed_;
        DebugDraw* debug_draw_;
        b2World world_;

        void Interpolate();
        void RestoreState();
        void SaveState();
    };
}  // namespace b2

#endif
