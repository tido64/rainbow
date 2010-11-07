/// Game object handles all in-game logic.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef ONWIREGAME_H_
#define ONWIREGAME_H_
#define OBJECT_COUNT 2

#include "Actions/WindAction.h"
#include "Framework/Physics.h"
#include "Objects/Building.h"
#include "Objects/HUD.h"
#include "Objects/Line.h"
#include "Objects/Skyline.h"

class OnWireGame
{
public:

	OnWireGame();
	~OnWireGame();

	void draw();

	/// Increments time by one second.
	void elapse_time();

	/// Returns the number of in-game objects.
	inline unsigned int get_game_object_count() { return OBJECT_COUNT; }

	/// Returns an array of all in-game objects.
	RealSprite **get_game_objects();

	/// Returns the number of HUD elements.
	inline unsigned int get_hud_element_count() { return this->hud->get_element_count(); }

	/// Returns an array of all HUD element objects.
	inline RealLabel **get_hud_elements() { return this->hud->get_elements(); }

	/// Returns the vertex array containing the line.
	inline GLfloat *get_line() { return this->line->line; }

	/// Game logic.
	void play();

	/// Resets all progress and sets a target distance.
	/// \param target Target distance
	void reset(const unsigned int target);

	/// Tells master how much the player has traveled since last time.
	/// \param distance Traveled distance since last call
	void travel(const unsigned int distance);

	void update();

private:
	bool finished;		///< Whether or not the player has reached the goal
	unsigned int time;	///< Time spent on the current game
	float
		target,			///< Target distance to reach the goal
		traveled;		///< Distance traveled by the player so far
	Building *building;	///< The target building
	HUD *hud;			///< The active HUD
	Line *line;			///< The in-game line
	Skyline *skyline;	///< Skyline backdrop
};

#endif
