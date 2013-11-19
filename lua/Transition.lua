--! Perform soft transitions of properties in one state to another.
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local module_path = (...):match("(.*[./\\])[^./\\]+") or ""

local __count = 0
local __transitions = {}

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

local function unregister(t)
	for i = 1, __count do
		if __transitions[i] == t then
			__transitions[i] = __transitions[__count]
			__count = __count - 1
			break
		end
	end
end

local Transition = {
	Functions = require(module_path .. "TransitionFunctions"),
	clear = clear,
	fadein = nil,
	fadeout = nil,
	fadeto = nil,
	move = nil,
	rotate = nil,
	scaleto = nil
}

setmetatable(Transition, {
	__update = function(dt)
		for i = __count, 1, -1 do
			__transitions[i]:tick(dt)
		end
	end
})

local function ticker(iterate, finalize, duration)
	local elapsed = 0
	return function(self, dt)
		elapsed = elapsed + dt
		local progress = elapsed / duration
		if progress >= 1.0 then
			finalize(self)
			unregister(self)
		else
			iterate(self, progress)
		end
	end
end

local LinearFunction = Transition.Functions.linear

do  -- fadein, fadeout
	local audio = rainbow.audio
	local play = audio.play
	local set_gain = audio.set_gain
	local stop = audio.stop

	local function create(source, iterate, finalize, duration)
		local self = {
			cancel = unregister,
			source = source,
			tick = ticker(iterate, finalize, duration),
			transition = LinearFunction
		}
		register(self)
		return self
	end

	local function in_finalize(self)
		set_gain(self.source, 1.0)
	end

	local function in_iterate(self, progress)
		set_gain(self.source, progress)
	end

	local function out_finalize(self)
		stop(self.source)
	end

	local function out_iterate(self, progress)
		set_gain(self.source, 1.0 - progress)
	end

	function Transition.fadein(source, duration)
		play(source)
		return create(source, in_iterate, in_finalize, duration)
	end

	function Transition.fadeout(source, duration)
		return create(source, out_iterate, out_finalize, duration)
	end
end

do  -- move
	local function finalize(self)
		self.move(self.node, self.x1 - self.x, self.y1 - self.y)
	end

	local function iterate(self, progress)
		local x = self.transition(self.x0, self.x1, progress)
		local y = self.transition(self.y0, self.y1, progress)
		self.move(self.node, x - self.x, y - self.y)
		self.x, self.y = x, y
	end

	local function move_drawable(drawable, x, y)
		drawable:move(x, y)
	end

	local function move_node(node, x, y)
		rainbow.scenegraph:move(node, x, y)
	end

	function Transition.move(node, x, y, duration, method)
		local self = {
			cancel = unregister,
			move = move_drawable,
			node = node,
			tick = ticker(iterate, finalize, duration),
			transition = method or LinearFunction,
			x = 0,
			y = 0,
			x0 = 0,
			y0 = 0,
			x1 = x,
			y1 = y
		}
		if type(node) == "table" then
			assert(node.get_position, "Object must implement :get_position()")
			assert(node.move, "Object must implement :move()")
			self.x0, self.y0 = node:get_position()
			self.x, self.y = self.x0, self.y0
		elseif type(node) == "userdata" then
			self.move = move_node
		else
			error("Invalid object")
		end
		register(self)
		return self
	end
end

do  -- rotate
	local function finalize(self)
		self.sprite:set_rotation(self.r1)
	end

	local function iterate(self, progress)
		self.sprite:set_rotation(self.transition(self.r0, self.r1, progress))
	end

	function Transition.rotate(sprite, r, duration, method)
		local self = {
			cancel = unregister,
			r0 = sprite:get_angle(),
			r1 = r,
			sprite = sprite,
			tick = ticker(iterate, finalize, duration),
			transition = method or LinearFunction
		}
		register(self)
		return self
	end
end

do  -- scaleto
	local function finalize(self)
		self.sprite:set_scale(self.final)
	end

	local function iterate(self, progress)
		self.sprite:set_scale(self.transition(self.start, self.final, progress))
	end

	function Transition.scaleto(sprite, start, final, duration, method)
		local self = {
			cancel = unregister,
			final = final,
			sprite = sprite,
			start = start,
			tick = ticker(iterate, finalize, duration),
			transition = method or LinearFunction
		}
		register(self)
		return self
	end
end

do  -- fadeto
	local function finalize(self)
		self.sprite:set_color(self.r, self.g, self.b, self.a1)
	end

	local function iterate(self, progress)
		self.sprite:set_color(self.r, self.g, self.b, self.transition(self.a, self.a1, progress))
	end

	function Transition.fadeto(sprite, alpha, duration, method)
		local self = {
			cancel = unregister,
			r = 255, g = 255, b = 255, a = 255,
			a1 = alpha,
			sprite = sprite,
			tick = ticker(iterate, finalize, duration),
			transition = method or LinearFunction
		}
		self.r, self.g, self.b, self.a = sprite:get_color()
		register(self)
		return self
	end
end

return rainbow.module.register(Transition)
