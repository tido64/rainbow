#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "LuaMachine.h"

/// Simple game loop for Lua-scripted games.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Director
{
public:
	//static Vector<Drawable *> drawables;  ///< Drawable objects

	Director();

	void draw();

	/// Load and initialise game script.
	void init(const char *const);

	/// Set screen resolution.
	void set_video(const int width, const int height);

	/// Update world.
	/// \param t  Current time or milliseconds since last frame (platform-dependent).
	void update(const unsigned long t);

private:
	LuaMachine lua;

	/// Intentionally left undefined.
	Director(const Director &);

	/// Intentionally left undefined.
	Director& operator=(const Director &);
};

#endif
