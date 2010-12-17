/// Game object handles all in-game logic.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef ONWIREGAME_H_
#define ONWIREGAME_H_

#include "Elements/Bird.h"
#include "Elements/Wind.h"
#include "Framework/Hardware/Controls.h"
#include "Framework/Random.h"
#include "Framework/Sprite.h"
#include "Framework/SpriteBatch.h"
#include "Framework/TextureAtlas.h"
#include "Objects/Assets.h"

typedef CCNode RealObject;

class OnWireGame
{
public:
	static const unsigned int element_count = 2;
	static const unsigned int object_count = 4;

	OnWireGame();
	~OnWireGame();

	void draw();

	/// Returns the number of in-game objects.
	inline unsigned int get_object_count() { return object_count; }

	/// Returns an array of all in-game objects. Z-ordered.
	RealObject **get_objects();

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

private:
	bool finished;      ///< Whether or not the player has reached the goal
	unsigned int time;  ///< Time spent on the current game
	float target;       ///< Target distance
	float traveled;     ///< Distance traveled by the player so far
	Random *random;     ///< Random number generator

	TextureAtlas texture_atlas;        ///< Main texture atlas
	Sprite *skyline;                   ///< The skyline backdrop
	Building building;                 ///< Target building
	SpriteBatch<2> background;         ///< Combines skyline and building
	Line line;                         ///< The line
	Avatar avatar;                     ///< Player avatar
	HUD hud;                           ///< Player's head-up display

	Element *elements[element_count];  ///< All elements
	Bird yellow_bird;
	Wind wind;

	//Font freetype;
};

#endif
