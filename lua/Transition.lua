-- Perform soft transitions of properties in one state to another.
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local module_path = (...):match("(.*[./\\])[^./\\]+") or ""
local Functions = require(module_path .. "TransitionFunctions")

local kDefaultTransitionFunction = Functions.linear

local __count = 0
local __transitions = {}
local __variadic = {
	function(start, desired, progress, transition)  -- 1
		return transition(start, desired, progress)
	end,
	function(start, desired, progress, transition)  -- 2
		return transition(start[1], desired[1], progress),
		       transition(start[2], desired[2], progress)
	end,
	function(start, desired, progress, transition)  -- 3
		return transition(start[1], desired[1], progress),
		       transition(start[2], desired[2], progress),
		       transition(start[3], desired[3], progress)
	end,
	function(start, desired, progress, transition)  -- 4
		return transition(start[1], desired[1], progress),
		       transition(start[2], desired[2], progress),
		       transition(start[3], desired[3], progress),
		       transition(start[4], desired[4], progress)
	end
}

local function clear()
	for i = 1, __count do
		__transitions[i] = nil
	end
	__count = 0
end

local function register(t)
	__count = __count + 1
	__transitions[__count] = t
end

local function unregister(i)
	__transitions[i] = __transitions[__count]
	__count = __count - 1
end

local function cancel(self)
	for i = 1, __count do
		if __transitions[i] == self then
			unregister(i)
			break
		end
	end
end

local Transition = {
	Functions = Functions,
	clear = clear,
	fadein = nil,
	fadeout = nil,
	fadeto = nil,
	move = nil,
	new = nil,
	rotate = nil,
	scaleto = nil
}

function Transition.new(target, func, start, desired, duration, transition)
	local elapsed = 0
	local iter = __variadic[type(desired) == "table" and #desired or 1]
	local min = math.min
	transition = transition or kDefaultTransitionFunction
	local self = {
		cancel = cancel,
		tick = function(dt)
			elapsed = elapsed + dt
			func(target, iter(start, desired, min(1.0, elapsed / duration), transition))
			return elapsed >= duration
		end
	}
	register(self)
	return self
end

if rainbow.audio then  -- Rainbow.ConFuoco
	local audio = rainbow.audio
	local play = audio.play
	local set_gain = audio.set_gain
	local stop = audio.stop

	local function set_gain_and_stop(self, progress)
		set_gain(self, progress)
		if progress >= 1.0 then
			stop(self)
		end
	end

	function Transition.fadein(source, duration)
		play(source)
		return Transition.new(source, set_gain, 0.0, 1.0, duration)
	end

	function Transition.fadeout(source, duration)
		return Transition.new(source, set_gain_and_stop, 1.0, 0.0, duration)
	end
else
	local function noop() end
	Transition.fadein = noop
	Transition.fadeout = noop
end

function Transition.fadeto(sprite, alpha, duration, transition)
	local func = function(self, alpha)
		local r, g, b, _ = self:get_color()
		self:set_color(r, g, b, alpha)
	end
	local _, _, _, a = sprite:get_color()
	return Transition.new(sprite, func, a, alpha, duration, transition)
end

function Transition.move(node, x, y, duration, transition)
	local x0, y0 = 0, 0
	local move = function(node, x, y)
		node:move(x - x0, y - y0)
		x0 = x
		y0 = y
	end
	if type(node) == "userdata" then
		-- Nodes don't have a metatable as opposed to e.g. sprites
		if not getmetatable(node) then
			local scenegraph = rainbow.scenegraph
			move = function(node, x, y)
				scenegraph:move(node, x - x0, y - y0)
				x0 = x
				y0 = y
			end
		end
	else
		assert(type(node) == "table", "Invalid object");
		assert(node.move, "Object must implement :move()")
	end
	return Transition.new(node, move, { x0, y0 }, { x, y }, duration, transition)
end

function Transition.rotate(sprite, r, duration, transition)
	return Transition.new(sprite, sprite.set_rotation, sprite:get_angle(), r, duration, transition)
end

function Transition.scaleto(sprite, factor, duration, transition)
	if type(factor) == "table" then
		if #factor == 1 then
			factor = { factor[1], factor[1] }
		end
	else
		factor = { factor, factor }
	end
	local x, y = sprite:get_scale()
	return Transition.new(sprite, sprite.set_scale, { x, y }, factor, duration, transition)
end

return rainbow.module.register(setmetatable(Transition, {
	__update = function(dt)
		for i = __count, 1, -1 do
			local t = __transitions[i]
			if t.tick(dt) and t == __transitions[i] then
				unregister(i)
			end
		end
	end
}))
