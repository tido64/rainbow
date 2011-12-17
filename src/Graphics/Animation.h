#ifndef ANIMATION_H_
#define ANIMATION_H_

#include "Common/ChronoTrigger.h"

class Sprite;

/// Sprite animation using sprite sheets.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Animation : public ChronoTrigger
{
public:
	/// \param s       The sprite to animate.
	/// \param frames  Null-terminated array of texture ids to be used as frames.
	/// \param fps     Frames per second.
	/// \param loop    Whether the animation should loop.
	inline Animation(Sprite *s, const unsigned int *const frames,
	                 const unsigned int fps, const bool loop = true);

	virtual ~Animation();

	/// Reset sprite animation.
	inline void reset();

	/// Set animation frames.
	/// \note This method takes ownership of the array.
	/// \param frames  Null-terminated array of texture ids to be used as frames.
	void set_frames(const unsigned int *const frames);

	/// Set looping.
	inline void set_loop(const bool loop);

	virtual void tick();

private:
	bool loop;                   ///< Whether this animation loops.
	const unsigned int *frame;   ///< Current frame.
	const unsigned int *frames;  ///< Null-terminated array of texture ids to be used as frames.
	Sprite *sprite;              ///< The sprite to animate.
};

Animation::Animation(Sprite *s, const unsigned int *const fs, const unsigned int fps, const bool loop) :
	ChronoTrigger(1000.0f / fps), loop(loop), frame(fs), frames(fs), sprite(s) { }

void Animation::reset()
{
	this->frame = this->frames;
}

void Animation::set_loop(const bool loop)
{
	this->loop = loop;
}

#endif
