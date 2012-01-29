--! Rainbow math helper.
--!
--! Note that if you're going to use any of these in a loop, it is better to
--! inline them, e.g.:
--!
--! \code
--! -- This is BAD!
--! function bad_code()
--! 	local d = 1.0;
--! 	while true do
--! 		d = rainbow.math.deg2rad(d);
--! 	end
--! end
--!
--! -- This is GOOD!
--! function good_code()
--! 	local d = 1.0;
--! 	local pi_over_180 = math.pi / 180;
--! 	while true do
--! 		d = d * pi_over_180;
--! 	end
--! end
--! \endcode
--!
--! Do also note that the loops in these examples are utterly useless.
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen
rainbow.math = rainbow.math or {};

--! Calculate the angle between two points with bearing north.
function rainbow.math.angle(a_x, a_y, b_x, b_y)
	return math.atan((b_y - a_y) / (b_x - a_x));
end

--! Convert degrees to radians.
function rainbow.math.deg2rad(degrees)
	return degrees * math.pi / 180;
end

--! Calculate the distance between two points.
--! \param a_x,a_y  Starting point.
--! \param b_x,b_y  End point.
function rainbow.math.distance(a_x, a_y, b_x, b_y)
	local s1 = b_x - a_x;
	local s2 = b_y - a_y;
	return math.sqrt(s1 * s1, s2 * s2);
end

--! Convert radians to degrees.
function rainbow.math.rad2deg(radians)
	return radians * 180 / math.pi;
end
