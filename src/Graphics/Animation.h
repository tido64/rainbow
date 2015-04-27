// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <functional>
#include <memory>

#include "Common/NonCopyable.h"
#include "Graphics/Sprite.h"

/// Sprite animation using sprite sheets.
class Animation : private NonCopyable<Animation>
{
public:
	enum class Event
	{
		Start,
		End,
		Complete
	};

	using Callback = std::function<void(const Event)>;
	using Frame = unsigned int;

	static const Frame kAnimationEnd;

	/// Constructs a sprite animation.
	/// \param sprite  The sprite to animate.
	/// \param frames  Array of texture ids to be used as frames, terminated
	///                with \c kAnimationEnd.
	/// \param fps     Frames per second.
	/// \param delay   Number of frames to delay before the animation loops.
	///                Negative numbers disable looping.
	Animation(const Sprite::Ref &sprite,
	          const Frame *const frames,
	          const unsigned int fps,
	          const int delay = 0);

	bool is_stopped() const { return stopped_; }

	void set_callback(Callback f) { callback_ = std::move(f); }

	/// Sets number of frames to delay before the animation loops. Negative
	/// numbers disable looping.
	void set_delay(const int delay) { delay_ = delay; }

	/// Sets playback rate in frames per second.
	void set_fps(const unsigned int fps);

	/// Sets animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Array of texture ids to be used as frames, terminated
	///                with \c kAnimationEnd.
	void set_frames(const Frame *const frames);

	/// Sets the sprite to animate.
	void set_sprite(const Sprite::Ref &sprite);

	/// Jumps to \p frame.
	void jump_to(const unsigned int frame);

	/// Rewinds animation.
	void rewind() { jump_to(0); }

	/// Starts animation if it was previously stopped. Always plays from the
	/// beginning.
	void start();

	/// Stops animation.
	void stop();

	/// Accumulates time and calls \c tick() when time is up.
	void update(const unsigned long dt);

private:
	bool stopped_;
	unsigned int accumulated_;  ///< Accumulated monotonic time.
	unsigned int interval_;     ///< Time till a tick.
	Frame frame_;               ///< Current frame.
	std::unique_ptr<const Frame[]> frames_;  ///< Array of texture ids to be used as frames, terminated with \c kAnimationEnd.
	Sprite::Ref sprite_;  ///< The sprite to animate.
	int delay_;           ///< Number of frames to delay before the animation loops. Negative numbers disable looping.
	int idled_;           ///< Number of frames idled.
	Callback callback_;

	void set_current_frame();

	/// Increments animation frame and resets/stops if the end is reached.
	void tick();
};

#endif
