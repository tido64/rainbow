--! Rainbow math helper.
--!
--! Because none of us have a PhD in math.
--!
--! Note that if you're going to use any of these in a loop, it is better to
--! inline them, e.g.:
--!
--! \code
--! -- This is BAD!
--! function bad_code()
--! 	local d = 1.0
--! 	while true do
--! 		d = rainbow.math.radians(d)
--! 	end
--! end
--!
--! -- This is GOOD!
--! function good_code()
--! 	local d = 1.0
--! 	local radians = rainbow.math.radians
--! 	while true do
--! 		d = radians(d)
--! 	end
--! end
--! \endcode
--!
--! Do also note that the loops in these examples are utterly useless.
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

rainbow.math = rainbow.math or {}

-- The universal gravitation constant, defined as G = 6.67384 * 10^-11 N(m/kg)^2.
G = 6.67384e-11

-- Standard gravitational acceleration, defined in m/s^2.
g = 9.80665

local kRadian = 180 / math.pi
local kDegree = math.pi / 180
local abs     = math.abs
local atan2   = math.atan2
local max     = math.max
local min     = math.min
local sqrt    = math.sqrt

--! Calculate the angle between two points.
function rainbow.math.angle(a_x, a_y, b_x, b_y)
	return atan2(b_y - a_y, b_x - a_x)
end

--! Clamp value between a range.
function rainbow.math.clamp(x, min_val, max_val)
	return min(max(x, min_val), max_val)
end

--! Convert radians to degrees.
function rainbow.math.degrees(radians)
	return radians * kRadian
end

--! Calculate the distance between two points.
--! \param a_x,a_y  Starting point.
--! \param b_x,b_y  End point.
function rainbow.math.distance(a_x, a_y, b_x, b_y)
	local s1 = b_x - a_x
	local s2 = b_y - a_y
	return sqrt(s1 * s1 + s2 * s2)
end

--! Newton's law of universal gravitation:
--! F = G * (m1 * m2) / r^2
function rainbow.math.gravitation(m1, m2, r)
	return G * m1 * m2 / (r * r)
end

--! Calculate the hitbox.
--! \param x,y           Centre of the box.
--! \param width,height  Dimension of the box.
--! \param scale         Scaling factor for on-screen box size.
function rainbow.math.hitbox(x, y, width, height, scale)
	scale = scale or 1.0
	local half_w = width * scale * 0.5
	local half_h = height * scale * 0.5
	local hitbox = {}
	hitbox.x0 = x - half_w
	hitbox.y0 = y - half_h
	hitbox.x1 = x + half_w
	hitbox.y1 = y + half_h
	return hitbox
end

--! Check whether a point is inside a box.
--! \param box    Table with the upper-left and lower-right points of the box.
--! \param point  The point to check.
function rainbow.math.is_inside(box, point)
	return point.x >= box.x0 and point.x <= box.x1 and point.y >= box.y0 and point.y <= box.y1
end

--! Convert degrees to radians.
function rainbow.math.radians(degrees)
	return degrees * kDegree
end
