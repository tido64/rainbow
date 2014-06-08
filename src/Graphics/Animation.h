// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <memory>

#include "Common/NonCopyable.h"
#include "Common/TimedEvent.h"
#include "Graphics/Sprite.h"

/// Sprite animation using sprite sheets.
class Animation : public TimedEvent<Animation>, private NonCopyable<Animation>
{
public:
	using Frame = unsigned int;

	static const Frame kAnimationEnd;

	/// Constructs a sprite animation.
	/// \param sprite  The sprite to animate.
	/// \param frames  Null-terminated array of texture ids to be used as
	///                frames.
	/// \param fps     Frames per second.
	/// \param delay   Number of ticks to delay before the animation loops.
	///                Negative numbers disable looping.
	Animation(const Sprite::Ref &sprite,
	          const Frame *const frames,
	          const unsigned int fps,
	          const int delay = 0);

	/// Sets number of ticks to delay before the animation loops. Negative
	/// numbers disable looping.
	inline void set_delay(const int delay);

	/// Sets animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Array of texture ids to be used as frames, terminated
	///                with \c kAnimationEnd.
	void set_frames(const Frame *const frames);

	/// Sets the sprite to animate.
	inline void set_sprite(const Sprite::Ref &sprite);

	/// Resets sprite animation.
	inline void reset();

	/// Increments animation frame and resets/stops if the end is reached.
	void tick();

private:
	Frame frame_;  ///< Current frame.
	Sprite::Ref sprite_;  ///< The sprite to animate.
	std::unique_ptr<const Frame[]> frames_;  ///< Array of texture ids to be used as frames, terminated with \c kAnimationEnd.
	int delay_;  ///< Number of ticks to delay before the animation loops. Negative numbers disable looping.
	int idled_;  ///< Number of ticks idled.
};

void Animation::set_delay(const int delay)
{
	delay_ = delay;
}

void Animation::set_sprite(const Sprite::Ref &sprite)
{
	sprite_ = sprite;
}

void Animation::reset()
{
	frame_ = 0;
	idled_ = 0;
}

#endif
