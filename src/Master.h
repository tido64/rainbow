/// Master object handles all game logic.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef MASTER_H_
#define MASTER_H_

#include "Actions/WindAction.h"
#include "Framework/Physics.h"
#include "Objects/HUD.h"
#include "Objects/Line.h"

class Master
{
public:

	/// Master is a singleton.
	static Master *Instance();
	~Master() { }

	/// Provides master with the HUD to update.
	/// \param hud The HUD in use
	void add(HUD *hud);

	/// Provides master with the in-game line.
	/// \param line The line in use
	void add(Line *line);

	/// Increments time by one second.
	void elapse_time();

	/// Returns player progression in percentage.
	/// \return Distance traveled divided by total distance
	float get_progress();

	/// Returns the duration of the current game.
	/// \return Time spent so far
	unsigned int get_time();

	/// Returns the distance traveled by the player.
	/// \return Traveled distance
	unsigned int get_traveled();

	/// Game logic.
	void play();

	/// Resets all progress and sets a target distance.
	/// \param target Target distance
	void reset(const unsigned int target);

	/// Tells master how much the player has traveled since last time.
	/// \param distance Traveled distance since last call
	void travel(const unsigned int distance);

private:
	bool finished;	///< Whether or not the player has reached the goal
	unsigned int
		target,		///< Target distance to reach the goal
		time,		///< Time spent on the current game
		traveled;	///< Distance traveled by the player so far
	HUD *hud;		///< The active HUD
	Line *line;		///< The in-game line

	Master() : target(0), time(target), traveled(target), hud(0), line(0) { }
};

#endif
