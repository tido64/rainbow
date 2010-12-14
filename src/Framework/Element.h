/// A generic game element.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "Sprite.h"

class Element
{
public:
	bool active;  ///< Whether this element is active

	Element() : active(false), sprite(0) { }
	Element(Sprite *s) : active(false), sprite(s) { }
	~Element() { delete this->sprite; }

	virtual void activate() = 0;
	virtual bool fire() = 0;

protected:
	Sprite *sprite;  ///< The element's sprite
};

#endif
