/// Simple game loop for Lua-scripted games.

/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "AssetManager.h"
#include "Lua.h"

class Director
{
public:
	Director();

	void draw();

	/// Loads and initialises game script
	inline void init(const char *game) { this->lua.load(game); }

	/// Increments time by one second.
	inline void tick() { this->lua.call("tick"); }

	void update();

private:
	Lua lua;
};

#endif
