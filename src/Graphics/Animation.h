// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <functional>
#include <memory>

#include "Common/NonCopyable.h"
#include "Common/TimedEvent.h"
#include "Graphics/Sprite.h"

/// Sprite animation using sprite sheets.
class Animation : public TimedEvent<Animation>, private NonCopyable<Animation>
{
	friend TimedEvent;

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

	void set_callback(Callback f) { callback_ = std::move(f); }

	/// Sets number of frames to delay before the animation loops. Negative
	/// numbers disable looping.
	void set_delay(const int delay) { delay_ = delay; }

	/// Sets playback rate in frames per second.
	void set_fps(const unsigned int fps) { set_timeout(1000 / fps); }

	/// Sets animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Array of texture ids to be used as frames, terminated
	///                with \c kAnimationEnd.
	void set_frames(const Frame *const frames);

	/// Sets the sprite to animate.
	void set_sprite(const Sprite::Ref &sprite) { sprite_ = sprite; }

	/// Resets sprite animation.
	void reset()
	{
		frame_ = 0;
		idled_ = 0;
	}

	/// Increments animation frame and resets/stops if the end is reached.
	void tick();

private:
	Frame frame_;  ///< Current frame.
	Sprite::Ref sprite_;  ///< The sprite to animate.
	std::unique_ptr<const Frame[]> frames_;  ///< Array of texture ids to be used as frames, terminated with \c kAnimationEnd.
	int delay_;  ///< Number of frames to delay before the animation loops. Negative numbers disable looping.
	int idled_;  ///< Number of frames idled.
	Callback callback_;

	void on_start();
	void on_stop();
};

#endif
