#ifndef DRAWABLE_H_
#define DRAWABLE_H_

/// A drawable interface.
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Drawable
{
public:
	virtual void draw() = 0;
	virtual void update() = 0;
};

#endif
