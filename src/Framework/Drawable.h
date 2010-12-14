/// A drawable interface.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef DRAWABLE_H_
#define DRAWABLE_H_

#include "Sprite.h"

class Drawable
{
public:
	Drawable() : sprite(0) { }
	~Drawable() { }

	/// Draws the sprite.
	inline void draw()
	{
		assert(this->sprite != 0);
		this->sprite->draw();
	}

	/// Sets the sprite to draw.
	virtual void set_sprite(Sprite *) = 0;

protected:
	Sprite *sprite;  ///< The drawable object
};

#endif
