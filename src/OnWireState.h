/// The state of OnWire.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ONWIRESTATE_H_
#define ONWIRESTATE_H_

class OnWireState
{
public:
	bool finished;      ///< Whether or not the player has reached the goal
	float target;       ///< Target distance
	float traveled;     ///< Distance traveled by the player so far
	unsigned int time;  ///< Time spent on the current game

	static inline OnWireState& Instance()
	{
		static OnWireState state;
		return state;
	}

	void reset(const unsigned int target)
	{
		this->finished = false;
		this->target = target;
		this->traveled = 0.0f;
		this->time = 0;
	}

private:
	OnWireState() : finished(false), target(0.0f), traveled(0.0f), time(0) { }
	OnWireState(const OnWireState &);
	OnWireState& operator=(const OnWireState &);
};

#endif
