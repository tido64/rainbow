/// Simple game loop for Lua-scripted games.

/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Lua.h"

class Director
{
public:
	Director();

	void draw();

	/// Loads and initialises game script
	void init(const char *);

	/// Handle key press events
	inline void key_press() { this->lua.input.update(this->lua.L); }

	void update(const float);

	/// Update screen resolution
	inline void update_video(const int w = 0, const int h = 0)
	{
		Screen::Instance().set(w, h);
		this->lua.platform.update(this->lua.L);
	}

private:
	Lua lua;
};

#endif
