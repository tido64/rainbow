// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_ACTOR_H_
#define SCRIPT_ACTOR_H_

#include "Script/Components/AnimationComponent.h"
#include "Script/Components/RenderComponent.h"
#include "Script/Components/ScriptComponent.h"

namespace rainbow
{
    class Actor
    {
    public:
        virtual ~Actor() {}

        auto animation_component() { return animation_component_impl(); }
        bool is_enabled() const { return enabled_; }
        bool is_mirrored() const { return render_component()->is_mirrored(); }
        auto position() const { return position_; }

        /// <summary>Sets local angle of rotation (in radian).</summary>
        void set_local_rotation(float r)
        {
            render_component()->set_local_rotation(r);
        }

        /// <summary>Sets uniform local scaling factor.</summary>
        /// <param name="f">Scaling factor for both axes.</param>
        void set_local_scale(float f)
        {
            render_component()->set_local_scale(f);
        }

        /// <summary>Sets non-uniform local scaling factor.</summary>
        /// <param name="f">Scaling factors for x- and y-axis.</param>
        void set_local_scale(const Vec2f& scale)
        {
            render_component()->set_local_scale(scale);
        }

        /// <summary>Sets actor position.</summary>
        void set_position(const Vec2f& p) { set_position_impl(p); }

        /// <summary>Disables actor.</summary>
        void disable()
        {
            enabled_ = false;
            on_disabled_impl();
        }

        /// <summary>Called on actor destruction.</summary>
        void dispose() { on_unload_impl(); }

        /// <summary>Enables actor.</summary>
        void enable()
        {
            enabled_ = true;
            on_enabled_impl();
        }

        /// <summary>Hides actor if it is currently shown.</summary>
        void hide() { render_component()->hide(); }

        /// <summary>Called on actor creation.</summary>
        void initialize()
        {
            initialize_components();
            on_initialized_impl();
        }

        /// <summary>Mirrors actor.</summary>
        void mirror() { render_component()->mirror(); }

        /// <summary>Moves actor by (x,y).</summary>
        void move(const Vec2f& distance) { move_impl(distance); }

        /// <summary>Shows actor if it is currently hidden.</summary>
        void show() { render_component()->show(); }

        /// <summary>Called every frame update.</summary>
        void update(uint64_t dt)
        {
            if (!is_enabled())
                return;

            script_component()->update(dt);
        }

    protected:
        auto render_component() -> RenderComponent*
        {
            return &render_component_;
        }

        auto render_component() const -> const RenderComponent*
        {
            return &render_component_;
        }

        auto script_component() -> IScriptComponent*
        {
            return script_component_impl();
        }

        void initialize_components()
        {
            initialize_render_component_impl();
            initialize_animation_component_impl();
            script_component()->initialize(*this);
        }

    private:
        bool enabled_ = true;
        Vec2f position_;
        RenderComponent render_component_;

        virtual auto animation_component_impl() -> IAnimationComponent*
        {
            return nullptr;
        }

        virtual auto script_component_impl() -> IScriptComponent*
        {
            static DummyScriptComponent dummy;
            return &dummy;
        }

        virtual void set_position_impl(const Vec2f& p)
        {
            render_component()->move(p - position_);
            position_ = p;
        }

        /// <summary>
        ///   Called when the animation component should be initialised.
        /// </summary>
        virtual void initialize_animation_component_impl() {}

        /// <summary>
        ///   Called when the render component should be initialised.
        /// </summary>
        virtual void initialize_render_component_impl() = 0;

        virtual void move_impl(const Vec2f& distance)
        {
            render_component()->move(distance);
            position_ += distance;
        }

        /// <summary>Called when actor has been disabled.</summary>
        virtual void on_disabled_impl() {}

        /// <summary>Called when actor has been enabled.</summary>
        virtual void on_enabled_impl() {}

        /// <summary>Called when all components have been initialised.</summary>
        virtual void on_initialized_impl() {}

        /// <summary>Called when actor is being disposed.</summary>
        virtual void on_unload_impl() {}
    };
}

#endif
