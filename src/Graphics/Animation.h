// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <memory>

#include "Common/Constants.h"
#include "Common/NonCopyable.h"
#include "Common/TimedEvent.h"
#include "Graphics/Sprite.h"

/// Sprite animation using sprite sheets.
class Animation : public TimedEvent<Animation>, private NonCopyable<Animation>
{
public:
	enum { kAnimationEnd = kMaxInt };

	/// Constructs a sprite animation.
	/// \param s       The sprite to animate.
	/// \param frames  Null-terminated array of texture ids to be used as
	///                frames.
	/// \param fps     Frames per second.
	/// \param delay   Number of ticks to delay before the animation loops.
	///                Negative numbers disable looping.
	Animation(const Sprite::Ref &sprite,
	          const unsigned int *const frames,
	          const unsigned int fps,
	          const int delay = 0);

	/// Sets number of ticks to delay before the animation loops. Negative
	/// numbers disable looping.
	inline void set_delay(const int delay);

	/// Sets animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Null-terminated array of texture ids to be used as
	///                frames.
	void set_frames(const unsigned int *const frames);

	/// Sets the sprite to animate.
	inline void set_sprite(const Sprite::Ref &sprite);

	/// Resets sprite animation.
	inline void reset();

	/// Increments animation frame and resets/stops if the end is reached.
	void tick();

private:
	unsigned int frame;  ///< Current frame.
	Sprite::Ref sprite;  ///< The sprite to animate.
	std::unique_ptr<const unsigned int[]> frames;  ///< Null-terminated array of texture ids to be used as frames.
	int delay;  ///< Number of ticks to delay before the animation loops. Negative numbers disable looping.
	int idled;  ///< Number of ticks idled.
};

void Animation::set_delay(const int delay)
{
	this->delay = delay;
}

void Animation::set_sprite(const Sprite::Ref &sprite)
{
	this->sprite = sprite;
}

void Animation::reset()
{
	this->frame = 0;
	this->idled = 0;
}

#endif
