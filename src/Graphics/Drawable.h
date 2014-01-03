#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

/// A drawable interface.
///
/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Drawable
{
public:
	static const char class_name[];

	virtual ~Drawable() = default;

	inline Drawable* get();

	inline void draw();
	inline void update();

private:
	virtual void draw_impl() = 0;
	virtual void update_impl() = 0;
};

Drawable* Drawable::get()
{
	return this;
}

void Drawable::draw()
{
	this->draw_impl();
}

void Drawable::update()
{
	this->update_impl();
}

#endif
