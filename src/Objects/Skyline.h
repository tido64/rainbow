/// Skyline sprite. Created and properly positioned on the screen.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef SKYLINE_H_
#define SKYLINE_H_

#include "../Framework/Sprite.h"
#include "../Hardware/Screen.h"

class Skyline
{
public:
	Skyline(Sprite *s);
	~Skyline() { delete this->sprite; }

private:
	Sprite *sprite;
};

#endif
