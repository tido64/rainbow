// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_GEOMETRY_H_
#define GRAPHICS_GEOMETRY_H_

#include "Common/Vec2.h"

namespace rainbow
{
	/// <summary>
	///   Applies function <paramref name="f"/> on every point on the edge of
	///   the circle with specified center and radius.
	/// </summary>
	/// <param name="center">Center of the circle.</param>
	/// <param name="radius">Radius of the circle.</param>
	/// <param name="num_segments">Number of segments making the circle.</param>
	/// <param name="f">The function to apply.</param>
	template <typename F>
	void for_each_point_on_circle_edge(const Vec2f& center,
	                                   const float radius,
	                                   const int num_segments,
	                                   F&& f)
	{
		const double increment = 2.0 * kPi / num_segments;
		const float sin_inc = std::sin(increment);
		const float cos_inc = std::cos(increment);
		Vec2f r0 = Vec2f::Left;
		const Vec2f start = center + radius * r0;
		Vec2f v0 = start;
		for (int i = 1; i < num_segments; ++i)
		{
			const Vec2f r1(cos_inc * r0.x - sin_inc * r0.y,
			               sin_inc * r0.x + cos_inc * r0.y);
			const Vec2f v1 = center + radius * r1;
			f(v0, v1);
			r0 = r1;
			v0 = v1;
		}
		f(v0, start);
	}
}

#endif
