// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

/// A drawable interface.
class Drawable
{
public:
	virtual ~Drawable() = default;

	inline Drawable* get();

	inline void draw();
	inline void update(const unsigned long dt);

private:
	virtual void draw_impl() = 0;
	virtual void update_impl(const unsigned long dt) = 0;
};

Drawable* Drawable::get()
{
	return this;
}

void Drawable::draw()
{
	draw_impl();
}

void Drawable::update(const unsigned long dt)
{
	update_impl(dt);
}

#endif
