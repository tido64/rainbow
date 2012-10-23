--! Transition of node properties.
--!
--! There is no clean way to implement this in C++ because it doesn't really
--! fit into the architecture anywhere. There are many situations where
--! transitions must be handled in some way or another, and the game engine
--! is not supposed to handle game logic.
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require("TransitionEffects")

rainbow.transition = rainbow.transition or {}

local __count = 0
local __list  = {}
local effects = rainbow.transition.effects

function rainbow.transition.clear()
	for i = 1, __count do
		__list[i] = nil
	end
	__count = 0
end

function rainbow.transition.update(dt)
	for i = __count, 1, -1 do
		__list[i]:tick(dt)
	end
end

local function register(t)
	__count = __count + 1
	__list[__count] = t
end

local function unregister(t)
	for i = __count, 1, -1 do
		if __list[i] == t then
			__list[i] = __list[__count]
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

	local function new(source, duration)
		local self = {
			cancel = unregister,
			transition = effects.linear
		}
		self.duration = duration
		self.elapsed = 0
		self.source = source
		register(self)
		return self
	end

	function rainbow.transition.fadein(source, duration)
		local self = new(source, duration)
		self.tick = fadein
		return self
	end

	function rainbow.transition.fadeout(source, duration)
		local self = new(source, duration)
		self.tick = fadeout
		return self
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

	function rainbow.transition.move(node, x, y, duration, method)
		method = method or effects.linear
		local self = {
			cancel = unregister,
			tick = move,
			transition = method
		}
		self.duration = duration
		self.elapsed = 0
		self.x1, self.y1 = x, y
		if type(node) == "table" then
			assert(node.get_position, "Object must implement :get_position()")
			self.move = function(sprite, x, y) sprite:move(x, y) end
			self.x0, self.y0 = node:get_position()
		elseif type(node) == "userdata" then
			self.move = function(node, x, y) rainbow.scenegraph:move(node, x, y) end
			self.x0, self.y0 = 0, 0
		else
			error("Invalid object")
		end
		self.x, self.y = self.x0, self.y0
		self.node = node
		register(self)
		return self
	end
end

do
	local function rotate(self, dt)
		self.elapsed = self.elapsed + dt
		local progress = self.elapsed / self.duration
		if progress >= 1.0 then
			self.sprite:set_rotation(self.final_r)
			unregister(self)
		else
			self.sprite:set_rotation(self.transition(self.r, self.final_r, progress))
		end
	end

	function rainbow.transition.rotate(sprite, r, duration, method)
		method = method or effects.linear
		local self = {
			cancel = unregister,
			tick = rotate,
			transition = method
		}
		self.duration = duration
		self.elapsed = 0
		self.final_r = r
		self.r = sprite:get_angle()
		self.sprite = sprite
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
		method = method or effects.linear
		local self = {
			cancel = unregister,
			tick = scaleto,
			transition = method
		}
		self.duration = duration
		self.elapsed = 0
		self.final = final
		self.start = start
		self.sprite = sprite
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
		method = method or effects.linear
		local self = {
			cancel = unregister,
			tick = fadeto,
			transition = method
		}
		self.duration = duration
		self.elapsed = 0
		self.final_a = alpha
		self.r, self.g, self.b, self.a = sprite:get_color()
		self.sprite = sprite
		register(self)
		return self
	end
end
