// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef THIRDPARTY_BOX2D_STABLEWORLD_H_
#define THIRDPARTY_BOX2D_STABLEWORLD_H_

// clang-format off
#include "ThirdParty/DisableWarnings.h"
#include <Box2D/Dynamics/b2World.h>  // NOLINT(llvm-include-order)
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

    class StableWorld final : public b2World, public DebuggableWorld
    {
    public:
        StableWorld(float gx = 0.0f, float gy = rainbow::kStandardGravity);
        ~StableWorld();

        // b2World overrides.

        void SetDebugDraw(b2Draw* debugDraw) /* override */;
        auto CreateBody(const b2BodyDef* def) -> b2Body* /* override */;
        void DestroyBody(b2Body* body) /* override */;
        void Step(float32 timeStep,
                  int32 velocityIterations,
                  int32 positionIterations) /* override */;

        // Implement DebuggableWorld.

        void DrawDebugData() override;

    private:
        float elapsed_;
        DebugDraw* debug_draw_;

        void Interpolate();
        void RestoreState();
        void SaveState();
    };
}  // namespace b2

#endif
