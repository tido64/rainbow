--! Transition effects.
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

rainbow.transition = rainbow.transition or {}
rainbow.transition.effects = {}

local sqrt = math.sqrt

function rainbow.transition.effects.linear(a, b, t)
	return a + (b - a) * t
end

function rainbow.transition.effects.easein_cubic(a, b, t)
	return a + (b - a) * t * t * t
end

function rainbow.transition.effects.easein_quadratic(a, b, t)
	return a + (b - a) * t * t * t * t
end

function rainbow.transition.effects.easein_square(a, b, t)
	return a + (b - a) * t * t
end

function rainbow.transition.effects.easeout_cubic(a, b, t)
	return a + (b - a) * t^(1 / 3)
end

function rainbow.transition.effects.easeout_quadratic(a, b, t)
	return a + (b - a) * t^(1 / 4)
end

function rainbow.transition.effects.easeout_square(a, b, t)
	return a + (b - a) * sqrt(t)
end
