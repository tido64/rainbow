/// Game object handles all in-game logic.

/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef ONWIREGAME_H_
#define ONWIREGAME_H_

#include <Rainbow/Hardware/Controls.h>
#include <Rainbow/SpriteBatch.h>

#include "Elements/Bird.h"
//#include "Elements/Rain.h"
#include "Elements/Wind.h"
#include "Objects/Assets.h"

class OnWireGame
{
public:
	OnWireGame();

	void draw();

	/// Returns the vertex array making up the line.
	inline GLfloat *get_line() { return this->line.vertices; }

	/// Resets all progress and sets a target distance.
	/// \param target Target distance
	void reset(const unsigned int target);

	/// Increments time by one second.
	void tick();

	/// Tells master how much the player has traveled since last time.
	/// \param distance Traveled distance since last call
	void travel(const unsigned int distance);

	void update();

protected:
	static const unsigned int element_count = 2;

private:
	bool finished;      ///< Whether or not the player has reached the goal
	unsigned int time;  ///< Time spent on the current game
	float target;       ///< Target distance
	float traveled;     ///< Distance traveled by the player so far

	TextureAtlas texture_atlas;        ///< Main texture atlas
	Sprite *skyline;                   ///< The skyline backdrop
	Building building;                 ///< Target building
	SpriteBatch<2> background;         ///< Combines skyline and building
	Line line;                         ///< The line
	Avatar avatar;                     ///< Player avatar
	SpriteBatch<1> foreground;         ///< Combines avatar and all game elements
	HUD hud;                           ///< Player's head-up display

	Element *elements[element_count];  ///< All elements
	Bird yellow_bird;
	//RainPS rain;
	Wind wind;
};

#endif
