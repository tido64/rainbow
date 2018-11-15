// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Box2D/StableWorld.h"

#include <Box2D/Dynamics/b2Body.h>

#include "ThirdParty/Box2D/DebugDraw.h"

using rainbow::Vec2f;

namespace
{
    constexpr float kFixedStep = 1.0f / 100.0f;
    constexpr float kStepsPerMs = 1.0f / kFixedStep / 1000.0f;
    constexpr int kMaxSteps = 10;

    template <typename F, typename... Args>
    void ForEachDynamicBody(b2Body* body, F&& f, Args&&... args)
    {
        for (auto b = body; b; b = b->GetNext())
        {
            if (b->GetType() == b2_staticBody)
                continue;

            f(b, std::forward<Args>(args)...);
        }
    }

    auto GetBodyState(b2Body* body)
    {
        return static_cast<b2::BodyState*>(body->GetUserData());
    }
}  // namespace

namespace b2
{
    BodyState::BodyState(const b2BodyDef* bd)
        : curr_p(bd->position), prev_p(bd->position), curr_r(bd->angle),
          prev_r(bd->angle)
    {
    }

    StableWorld::StableWorld(float gx, float gy)
        : b2World(b2Vec2(gx, gy)), elapsed_(0.0), debug_draw_(nullptr)
    {
        SetAutoClearForces(false);  // TODO: Needed?
    }

    StableWorld::~StableWorld() { SetDebugDraw(nullptr); }

    void StableWorld::SetDebugDraw(b2Draw* debugDraw)
    {
        if (debug_draw_ != nullptr)
            debug_draw_->Remove(this);
        b2World::SetDebugDraw(debugDraw);
        debug_draw_ = static_cast<DebugDraw*>(debugDraw);
        if (debug_draw_ != nullptr)
            debug_draw_->Add(this);
    }

    auto StableWorld::CreateBody(const b2BodyDef* bd) -> b2Body*
    {
        auto body = b2World::CreateBody(bd);
        body->SetUserData(new BodyState(bd));
        return body;
    }

    void StableWorld::DestroyBody(b2Body* body)
    {
        delete GetBodyState(body);
        b2World::DestroyBody(body);
    }

    void StableWorld::Step(float32 timeStep,
                           int32 velocityIterations,
                           int32 positionIterations)
    {
        elapsed_ += timeStep;
        const int steps = static_cast<int>(elapsed_ * kStepsPerMs);
        if (steps == 0)
            RestoreState();
        else
        {
            elapsed_ -= steps / kStepsPerMs;
            for (int i = 0; i < std::min(steps, kMaxSteps); ++i)
            {
                b2World::Step(
                    kFixedStep, velocityIterations, positionIterations);
                SaveState();
            }
            ClearForces();  // TODO: Needed?
            Interpolate();
        }
    }

    void StableWorld::DrawDebugData() { b2World::DrawDebugData(); }

    void StableWorld::Interpolate()
    {
        const float ratio = elapsed_ * kStepsPerMs;
        ForEachDynamicBody(
            GetBodyList(),
            [this](b2Body* body, float ratio, float rest) {
                if (!body->IsAwake())
                    return;

                auto d = GetBodyState(body);
                if (!d->sprite)
                    return;

                const b2Vec2 v = ptm_ * (ratio * d->curr_p + rest * d->prev_p);
                d->sprite->set_position(Vec2f(v.x, v.y));
                d->sprite->set_rotation(ratio * d->curr_r + rest * d->prev_r);
            },
            ratio,
            1.0f - ratio);
    }

    void StableWorld::RestoreState()
    {
        ForEachDynamicBody(GetBodyList(), [this](b2Body* body) {
            auto d = GetBodyState(body);
            if (!d->sprite)
                return;

            const Vec2f position(d->curr_p.x * ptm_, d->curr_p.y * ptm_);
            d->sprite->set_position(position);
            d->sprite->set_rotation(d->curr_r);
        });
    }

    void StableWorld::SaveState()
    {
        ForEachDynamicBody(GetBodyList(), [](b2Body* body) {
            auto d = GetBodyState(body);
            const auto& t = body->GetTransform();
            d->prev_p = d->curr_p;
            d->curr_p = t.p;
            d->prev_r = d->curr_r;
            d->curr_r = t.q.GetAngle();
        });
    }
}  // namespace b2
