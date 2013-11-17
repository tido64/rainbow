-- Rainbow math helper.
--
-- Because none of us have a PhD in math (or physics).
--
-- Note that if you're going to use any of these in a loop, it is better to
-- inline them, e.g.:
--
-- \code
-- local Math = require(module_path .. "Math")
--
-- function bad_code()
--   local d = 1.0
--   while true do
--     d = Math.radians(d)  -- This is BAD!
--   end
-- end
--
-- function good_code()
--   local d = 1.0
--   local radians = Math.radians  -- This is GOOD!
--   while true do
--     d = radians(d)
--   end
-- end
-- \endcode
--
-- Do also note that the loops in these examples are utterly useless.
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local atan2   = math.atan2
local max     = math.max
local min     = math.min
local sqrt    = math.sqrt

local kDegree = math.pi / 180
local kRadian = 180 / math.pi

-- The universal gravitation constant, defined as G = 6.67384 * 10^-11 N(m/kg)^2.
local G = 6.67384e-11

-- Standard gravitational acceleration, defined in m/s^2.
local g = 9.80665

--! Calculate the angle between two points.
local function angle(ax, ay, bx, by)
	return atan2(by - ay, bx - ax)
end

--! Clamp value between a range.
local function clamp(x, min_val, max_val)
	return min(max(x, min_val), max_val)
end

--! Convert radians to degrees.
local function degrees(radians)
	return radians * kRadian
end

--! Calculate the distance between two points.
--! \param ax,ay  Starting point.
--! \param bx,by  End point.
local function distance(ax, ay, bx, by)
	local s1 = bx - ax
	local s2 = by - ay
	return sqrt(s1 * s1 + s2 * s2)
end

--! Newton's law of universal gravitation:
--! F = G * (m1 * m2) / r^2
local function gravitation(m1, m2, r)
	return G * m1 * m2 / (r * r)
end

--! Calculate the hitbox.
--! \param x,y           Centre of the box.
--! \param width,height  Dimension of the box.
--! \param scale         Scaling factor for on-screen box size.
local function hitbox(x, y, width, height, scale)
	scale = scale or 1.0
	local half_w = width * scale * 0.5
	local half_h = height * scale * 0.5
	return {
		x0 = x - half_w,
		y0 = y - half_h,
		x1 = x + half_w,
		y1 = y + half_h
	}
end

--! Check whether a point is inside a box.
--! \param box    Table with the upper-left and lower-right points of the box.
--! \param point  The point to check.
local function is_inside(box, point)
	return point.x >= box.x0 and point.x <= box.x1 and point.y >= box.y0 and point.y <= box.y1
end

--! Convert degrees to radians.
local function radians(degrees)
	return degrees * kDegree
end

return {
	G = G,
	angle = angle,
	clamp = clamp,
	degrees = degrees,
	distance = distance,
	g = g,
	gravitation = gravitation,
	hitbox = hitbox,
	is_inside = is_inside,
	radians = radians
}
