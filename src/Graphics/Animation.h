// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <functional>
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

	using Callback = std::function<void(Animation *, const Event)>;
	using Frame = unsigned int;

	static const Frame kAnimationEnd;

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
	Animation(const Sprite::Ref &sprite,
	          const Frame *const frames,
	          const unsigned int fps,
	          const int delay = 0);

	/// <summary>
	///   Returns the currently displayed frame; <c>kAnimationEnd</c> if none.
	/// </summary>
	unsigned int current_frame() const { return frame_ - 1; }

	/// <summary>Returns whether animation is stopped.</summary>
	bool is_stopped() const { return stopped_; }

	/// <summary>
	///   Sets callback for start, end, and complete (loop) events.
	/// </summary>
	void set_callback(Callback f) { callback_ = std::move(f); }

	/// <summary>
	///   Sets number of frames to delay before the animation loops. Negative
	///   numbers disable looping.
	/// </summary>
	void set_delay(const int delay) { delay_ = delay; }

	/// <summary>Sets playback rate in frames per second.</summary>
	void set_fps(const unsigned int fps);

	/// <summary>Sets animation frames.</summary>
	/// <remarks>This method takes ownership of the array.</remarks>
	/// <param name="frames">
	///   Array of texture ids to be used as frames, terminated with
	///   <c>kAnimationEnd</c>.
	/// </param>
	void set_frames(const Frame *const frames);

	/// <summary>Sets the sprite to animate.</summary>
	void set_sprite(const Sprite::Ref &sprite);

	/// <summary>Jumps to <paramref name="frame"/>.</summary>
	void jump_to(const unsigned int frame);

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
	void update(const unsigned long dt);

private:
	bool stopped_;
	unsigned int accumulated_;  ///< Accumulated monotonic time.
	unsigned int interval_;     ///< Time till a tick.
	unsigned int frame_;        ///< Current frame.
	std::unique_ptr<const Frame[]> frames_;  ///< Array of texture ids to be used as frames, terminated with <c>kAnimationEnd</c>.
	Sprite::Ref sprite_;  ///< The sprite to animate.
	int delay_;           ///< Number of frames to delay before the animation loops. Negative numbers disable looping.
	int idled_;           ///< Number of frames idled.
	Callback callback_;   ///< Event callback.

	void set_current_frame();

	/// <summary>
	///   Increments animation frame and resets/stops if the end is reached.
	/// </summary>
	void tick();
};

#endif
