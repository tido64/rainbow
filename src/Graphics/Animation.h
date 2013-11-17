// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_ANIMATION_H_
#define GRAPHICS_ANIMATION_H_

#include <memory>

#include "Common/Constants.h"
#include "Common/NonCopyable.h"
#include "Common/TimedEvent.h"

class Sprite;

typedef unsigned int uint_t;

/// Sprite animation using sprite sheets.
class Animation : public TimedEvent<Animation>, private NonCopyable<Animation>
{
public:
	enum { kAnimationEnd = kMaxInt };

	/// \param s       The sprite to animate.
	/// \param frames  Null-terminated array of texture ids to be used as frames.
	/// \param fps     Frames per second.
	/// \param delay   Number of ticks to delay before the animation loops. Negative numbers disable looping.
	inline Animation(Sprite *s, const uint_t *const frames, const uint_t fps, const int delay = 0);

	/// Resets sprite animation.
	inline void reset();

	/// Sets number of ticks to delay before the animation loops. Negative
	/// numbers disable looping.
	inline void set_delay(const int delay);

	/// Sets animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Null-terminated array of texture ids to be used as frames.
	void set_frames(const uint_t *const frames);

	/// Sets the sprite to animate.
	inline void set_sprite(Sprite *);

	/// Increments animation frame and resets/stops if the end is reached.
	void tick();

private:
	int delay;            ///< Number of ticks to delay before the animation loops. Negative numbers disable looping.
	int idled;            ///< Number of ticks idled.
	const uint_t *frame;  ///< Current frame.
	std::unique_ptr<const uint_t[]> frames;  ///< Null-terminated array of texture ids to be used as frames.
	Sprite *sprite;       ///< The sprite to animate.
};

Animation::Animation(Sprite *s, const uint_t *const fs, const uint_t fps, const int d) :
	TimedEvent(1000.0f / fps), delay(d), idled(0), frame(fs), frames(fs), sprite(s) { }

void Animation::reset()
{
	this->frame = this->frames.get();
	this->idled = 0;
}

void Animation::set_delay(const int delay)
{
	this->delay = delay;
}

void Animation::set_sprite(Sprite *s)
{
	this->sprite = s;
}

#endif
