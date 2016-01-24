// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <functional>
#include <limits>
#include <memory>

#include "Common/NonCopyable.h"
#include "Graphics/Sprite.h"

/// <summary>Sprite animation using sprite sheets.</summary>
class Animation : private NonCopyable<Animation>
{
public:
    enum class Event
    {
        Start,
        End,
        Complete,
        Frame
    };

    using Callback = std::function<void(Animation*, Event)>;
    using Frame = unsigned int;
    using Frames = std::unique_ptr<const Frame[]>;

    static constexpr Frame kAnimationEnd = std::numeric_limits<Frame>::max();

    /// <summary>Constructs a sprite animation.</summary>
    /// <param name="sprite">The sprite to animate.</param>
    /// <param name="frames">
    ///   Array of texture ids to be used as frames, terminated with
    ///   <c>kAnimationEnd</c>.
    /// </param>
    /// <param name="fps">Frames per second.</param>
    /// <param name="delay">
    ///   Number of frames to delay before the animation loops. Negative numbers
    ///   disable looping.
    /// </param>
    Animation(const Sprite::Ref& sprite,
              Frames frames,
              unsigned int fps,
              int delay = 0);

    /// <summary>Returns the currently displayed frame.</summary>
    auto current_frame() const { return frame_; }

    /// <summary>Returns the frame rate in frames per second.</summary>
    auto frame_rate() const -> unsigned int { return 1000 / interval_; }

    /// <summary>Returns whether animation is stopped.</summary>
    bool is_stopped() const { return stopped_; }

    /// <summary>Returns the target sprite.</summary>
    auto sprite() const { return sprite_; }

    /// <summary>
    ///   Sets callback for start, end, and complete (loop) events.
    /// </summary>
    void set_callback(Callback f) { callback_ = std::move(f); }

    /// <summary>
    ///   Sets number of frames to delay before the animation loops. Negative
    ///   numbers disable looping.
    /// </summary>
    void set_delay(int delay) { delay_ = delay; }

    /// <summary>Sets the frame rate in frames per second.</summary>
    void set_frame_rate(unsigned int fps);

    /// <summary>Sets animation frames.</summary>
    /// <remarks>This method takes ownership of the array.</remarks>
    /// <param name="frames">
    ///   Array of texture ids to be used as frames, terminated with
    ///   <c>kAnimationEnd</c>.
    /// </param>
    void set_frames(Frames frames);

    /// <summary>Sets the sprite to animate.</summary>
    void set_sprite(const Sprite::Ref& sprite);

    /// <summary>Jumps to <paramref name="frame"/>.</summary>
    void jump_to(unsigned int frame);

    /// <summary>
    ///   Releases ownership of animation frames and returns it.
    /// </summary>
    const Frame* release() { return frames_.release(); }

    /// <summary>Rewinds animation.</summary>
    void rewind() { jump_to(0); }

    /// <summary>Starts animation if it was previously stopped.</summary>
    /// <remarks>Always plays from the beginning.</remarks>
    void start();

    /// <summary>Stops animation.</summary>
    void stop();

    /// <summary>
    ///   Must be called every frame for as long as it's meant to be active.
    /// </summary>
    void update(unsigned long dt);

private:
    bool stopped_;
    unsigned int accumulated_;  ///< Accumulated monotonic time.
    unsigned int interval_;     ///< Time till a tick.
    unsigned int frame_;        ///< Current frame.
    Frames frames_;             ///< Array of texture ids to be used as frames, terminated with <c>kAnimationEnd</c>.
    Sprite::Ref sprite_;        ///< The sprite to animate.
    int delay_;                 ///< Number of frames to delay before the animation loops. Negative numbers disable looping.
    int idled_;                 ///< Number of frames idled.
    Callback callback_;         ///< Event callback.

    void set_current_frame();

    /// <summary>
    ///   Increments animation frame and resets/stops if the end is reached.
    /// </summary>
    void tick();
};

#endif
