// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_SCENECOMPONENT_H_
#define SCRIPT_COMPONENTS_SCENECOMPONENT_H_

#include "Common/NonCopyable.h"
#include "Graphics/SceneGraph.h"

namespace rainbow
{
    class SceneComponent : private NonCopyable<SceneComponent>
    {
    public:
        SceneComponent() : node_(rainbow::SceneNode::create().release()) {}
        virtual ~SceneComponent() { node_->remove(); }

        bool is_enabled() const { return node_->is_enabled(); }
        rainbow::SceneNode* node() const { return node_; }

        Vec2f position() const { return position_; }
        void set_position(const Vec2f& p) { move(p - position_); }

        void disable() { node_->set_enabled(false); }
        void enable() { node_->set_enabled(true); }

        virtual void move(const Vec2f& delta) { position_ += delta; }

    private:
        rainbow::SceneNode* node_;  // Scene graph node.
        Vec2f position_;            // Logical position.
    };
}

#endif
