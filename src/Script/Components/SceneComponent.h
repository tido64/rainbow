// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_SCENECOMPONENT_H_
#define SCRIPT_COMPONENTS_SCENECOMPONENT_H_

#include "Common/NonCopyable.h"

namespace rainbow
{
    class SceneComponent : private NonCopyable<SceneComponent>
    {
    public:
        virtual ~SceneComponent() {}

        auto position() const -> const Vec2f& { return position_; }
        void set_position(const Vec2f& p) { move(p - position_); }

        void move(const Vec2f& distance)
        {
            position_ += distance;
            move_impl(distance);
        }

    private:
        Vec2f position_;   // Logical position.

        virtual void move_impl(const Vec2f&) {}
    };
}

#endif
