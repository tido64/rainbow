--! Transition of node properties.
--!
--! There is no clean way to implement this in C++ because it doesn't really
--! fit into the architecture anywhere. There are many situations where
--! transitions must be handled in some way or another, and the game engine
--! is not supposed to handle game logic.
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local __count = 0
local __transitions  = {}

rainbow.transition = {
	clear = nil,
	effects = nil,
	fadein = nil,
	fadeout = nil,
	fadeto = nil,
	move = nil,
	rotate = nil,
	scaleto = nil
}
require("TransitionEffects")
local effects = rainbow.transition.effects

function rainbow.transition.clear()
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
	for i = __count, 1, -1 do
		if __transitions[i] == t then
			__transitions[i] = __transitions[__count]
			__count = __count - 1
			break
		end
	end
end

do
	local audio = rainbow.audio

	local function fadein(self, dt)
		if self.elapsed == 0 then
			audio.play(self.source)
		end
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			unregister(self)
		else
			audio.set_gain(self.source, progress)
		end
	end

	local function fadeout(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			audio.stop(self.source)
			unregister(self)
		else
			audio.set_gain(self.source, 1.0 - progress)
		end
	end

	local function new(source, duration, fade)
		local self = {
			cancel = unregister,
			duration = duration,
			elapsed = 0,
			source = source,
			tick = fade,
			transition = effects.linear
		}
		register(self)
		return self
	end

	function rainbow.transition.fadein(source, duration)
		return new(source, duration, fadein)
	end

	function rainbow.transition.fadeout(source, duration)
		return new(source, duration, fadeout)
	end
end

do
	local function move(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			self.move(self.node, self.x1 - self.x, self.y1 - self.y)
			unregister(self)
		else
			local x = self.transition(self.x0, self.x1, progress)
			local y = self.transition(self.y0, self.y1, progress)
			self.move(self.node, x - self.x, y - self.y)
			self.x, self.y = x, y
		end
	end

	local function move_sprite(sprite, x, y)
		sprite:move(x, y)
	end

	local function move_node(node, x, y)
		rainbow.scenegraph:move(node, x, y)
	end

	function rainbow.transition.move(node, x, y, duration, method)
		local self = {
			cancel = unregister,
			duration = duration,
			elapsed = 0,
			node = node,
			tick = move,
			transition = method or effects.linear,
			x1 = x,
			y1 = y
		}
		if type(node) == "table" then
			assert(node.get_position, "Object must implement :get_position()")
			self.move = move_sprite
			self.x0, self.y0 = node:get_position()
		elseif type(node) == "userdata" then
			self.move = move_node
			self.x0, self.y0 = 0, 0
		else
			error("Invalid object")
		end
		self.x, self.y = self.x0, self.y0
		register(self)
		return self
	end
end

do
	local function rotate(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			self.sprite:set_rotation(self.r1)
			unregister(self)
		else
			self.sprite:set_rotation(self.transition(self.r0, self.r1, progress))
		end
	end

	function rainbow.transition.rotate(sprite, r, duration, method)
		local self = {
			cancel = unregister,
			duration = duration,
			elapsed = 0,
			r0 = sprite:get_angle(),
			r1 = r,
			sprite = sprite,
			tick = rotate,
			transition = method or effects.linear
		}
		register(self)
		return self
	end
end

do
	local function scaleto(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			self.sprite:set_scale(self.final)
			unregister(self)
		else
			self.sprite:set_scale(self.transition(self.start, self.final, progress))
		end
	end

	function rainbow.transition.scaleto(sprite, start, final, duration, method)
		local self = {
			cancel = unregister,
			duration = duration,
			elapsed = 0,
			final = final,
			sprite = sprite,
			start = start,
			tick = scaleto,
			transition = method or effects.linear
		}
		register(self)
		return self
	end
end

do
	local function fadeto(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			self.sprite:set_color(self.r, self.g, self.b, self.final_a)
			unregister(self)
		else
			self.sprite:set_color(self.r, self.g, self.b, self.transition(self.a, self.final_a, progress))
		end
	end

	function rainbow.transition.fadeto(sprite, alpha, duration, method)
		local self = {
			cancel = unregister,
			duration = duration,
			elapsed = 0,
			final_a = alpha,
			sprite = sprite,
			tick = fadeto,
			transition = method or effects.linear
		}
		self.r, self.g, self.b, self.a = sprite:get_color()
		register(self)
		return self
	end
end

setmetatable(rainbow.transition, {
	__update = function(dt)
		for i = __count, 1, -1 do
			__transitions[i]:tick(dt)
		end
	end
})

rainbow.module.register(rainbow.transition)
