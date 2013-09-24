--! Transition effects.
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local sqrt = math.sqrt

local Functions = {
	linear = function(a, b, t)
		return a + (b - a) * t
	end,

	easein_cubic = function(a, b, t)
		return a + (b - a) * t * t * t
	end,

	easein_quadratic = function(a, b, t)
		return a + (b - a) * t * t * t * t
	end,

	easein_square = function(a, b, t)
		return a + (b - a) * t * t
	end,

	easeout_cubic = function(a, b, t)
		return a + (b - a) * t^(1 / 3)
	end,

	easeout_quadratic = function(a, b, t)
		return a + (b - a) * t^(1 / 4)
	end,

	easeout_square = function(a, b, t)
		return a + (b - a) * sqrt(t)
	end,
}

return Functions
