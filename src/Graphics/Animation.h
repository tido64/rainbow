#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <ctime>

/// A generic animation class.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Animation
{
public:
	/// Create a Animation of a property from an initial value to a final value.
	/// \param duration  Duration in milliseconds. Default: 1000ms.
	Animation(const unsigned int duration);
	virtual ~Animation() { }

	bool is_stopped() const;

	/// Reset animation to the beginning.
	void reset();

	/// Stop animation.
	void stop();

	/// Update the property with a new value.
	/// \return true when the Animation is completed.
	virtual bool update() = 0;

protected:
	float get_progress();

private:
	bool stopped;
	const float duration;  ///< Duration of the Animation (inversed)
	clock_t tm_start;      ///< Time at which the Animation started
};

inline Animation::Animation(const unsigned int duration) :
	stopped(false), duration(1.0f / duration), tm_start(0) { }

inline bool Animation::is_stopped() const
{
	return this->stopped;
}

inline void Animation::reset()
{
	this->stopped = false;
	this->tm_start = 0;
}

inline void Animation::stop()
{
	this->stopped = true;
}

#endif
