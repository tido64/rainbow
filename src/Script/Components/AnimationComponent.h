// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_COMPONENTS_ANIMATIONCOMPONENT_H_
#define SCRIPT_COMPONENTS_ANIMATIONCOMPONENT_H_

#include <array>
#include <unordered_map>

#include "Graphics/Animation.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/Sprite.h"
#include "Script/Components/RenderComponent.h"

namespace rainbow
{
    /// <summary>Sprite animation controller.</summary>
    /// <remarks>
    ///   A sprite attached to an animation controller is guaranteed to have
    ///   only one animation playing at any time, given no external
    ///   manipulations.
    /// </remarks>
    class Animator
    {
    public:
        Animator() : active_(-1), animation_(Sprite::Ref(), nullptr, 1) {}

        ~Animator()
        {
            animation_.set_callback(nullptr);
            release();
        }

        /// <summary>Returns the id of the currently active animation.</summary>
        auto active() const { return active_; }

        /// <summary>Returns the animation object.</summary>
        auto animation() -> Animation& { return animation_; }

        /// <summary>Returns the animation object.</summary>
        auto animation() const -> const Animation& { return animation_; }

        /// <summary>
        ///   Adds an animation with given name to the collection.
        /// </summary>
        /// <param name="name">Name of the animation.</param>
        /// <param name="frames">
        ///   Array of texture ids to be used as frames, terminated with
        ///   <see cref="Animation::kAnimationEnd"/>. Caller must make sure the
        ///   array outlives the component.
        /// </param>
        /// <param name="frame_rate">Frame rate in frames per second.</param>
        /// <param name="delay">
        ///   Optional. Number of frames to delay before the animation loops.
        ///   Negative numbers disable looping. Defaults to 0.
        /// </param>
        /// <param name="callback">
        ///   Optional. Callback for start, end, and complete (loop) events.
        /// </param>
        void add(int name,
                 const Animation::Frame* frames,
                 unsigned int frame_rate,
                 int delay = 0,
                 Animation::Callback callback = nullptr)
        {
            animations_[name] = Animation{frame_rate, frames, delay, callback};
        }

        /// <summary>
        ///   Returns whether the animation with given id is currently active.
        /// </summary>
        bool is_active(int name) const { return name == active_; }

        /// <summary>Sets the animation to play.</summary>
        /// <param name="name">Name of the animation to set.</param>
        /// <returns>A reference to the newly set animation.</returns>
        auto set(int name) -> Animation&
        {
            return set(name, animations_.at(name));
        }

    private:
        struct Animation
        {
            unsigned int frame_rate;
            const ::Animation::Frame* frames;
            int delay;
            ::Animation::Callback callback;
        };

        int active_;
        ::Animation animation_;
        std::unordered_map<int, Animation> animations_;

        void release()
        {
            animation_.stop();
            animation_.release();
        }

        auto set(int name, const Animation& anim) -> ::Animation&
        {
            release();
            animation_.set_frame_rate(anim.frame_rate);
            animation_.set_frames(::Animation::Frames(anim.frames));
            animation_.set_delay(anim.delay);
            animation_.set_callback(anim.callback);
            animation_.start();
            active_ = name;
            return animation_;
        }
    };

    /// <summary>Component for controlling <see cref="Animator"/>s.</summary>
    class IAnimationComponent
    {
    public:
        /// <summary>
        ///   Returns the name of the currently active animation.
        /// </summary>
        virtual auto active() const -> int = 0;

        /// <summary>Returns the <see cref="Animator"/> at index.</summary>
        virtual auto animator(size_t i) -> Animator& = 0;

        /// <summary>Sets the animation to play.</summary>
        /// <param name="name">Name of the animation to set.</param>
        /// <returns>A reference to the newly set animation.</returns>
        virtual auto set(int name) -> Animation& = 0;

        /// <summary>Stops all animations.</summary>
        virtual void stop() = 0;

    protected:
        virtual ~IAnimationComponent() = default;
    };

    template <size_t N>
    class AnimationComponent : public IAnimationComponent
    {
    public:
        auto active() const -> int final { return active_; }
        auto animator(size_t i) -> Animator& final { return animators_.at(i); }

        void initialize(RenderComponent& render, SceneNode& parent)
        {
            R_ASSERT(render.batch().size() >= N,
                     "At least one sprite per animator required.");

            for (size_t i = 0; i < N; ++i)
            {
                auto& animation = animators_[i].animation();
                animation.set_sprite(render.sprite(i));
                parent.add_child(animation);
            }
        }

        auto set(int name) -> Animation& override
        {
            auto& animator = animators_[0];
            if (name == active_)
                return animator.animation();

            active_ = name;
            animator.set(name);
            return animator.animation();
        }

        void stop() final
        {
            active_ = -1;
            for (auto&& animator : animators_)
                animator.animation().stop();
        }

    private:
        int active_ = -1;
        std::array<Animator, N> animators_;
    };
}

#endif
