#ifndef DRAWABLE_H_
#define DRAWABLE_H_

/// A drawable interface.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Drawable
{
public:
	static const char class_name[];

	virtual ~Drawable() { }

	Drawable* raw_ptr() { return this; }

	virtual void draw() = 0;
	virtual void update() = 0;
};

#endif
