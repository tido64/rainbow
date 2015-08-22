// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_DRAWABLE_H_
#define GRAPHICS_DRAWABLE_H_

#include "Common/Vec2.h"

/// <summary>A drawable interface.</summary>
class Drawable
{
public:
	virtual ~Drawable() = default;

	Drawable* get() { return this; }

	void move(const Vec2f &delta) { move_impl(delta); }

	void draw() { draw_impl(); }
	void update(const unsigned long dt) { update_impl(dt); }

private:
	virtual void move_impl(const Vec2f &) {}

	virtual void draw_impl() = 0;
	virtual void update_impl(const unsigned long dt) = 0;
};

#endif
