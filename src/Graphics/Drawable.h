#ifndef DRAWABLE_H_
#define DRAWABLE_H_

/// A drawable interface.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Drawable
{
public:
	virtual ~Drawable() { }
	virtual void draw() = 0;
	virtual void update() = 0;
};

#endif
