--! Transition of sprite properties.
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
			self.sprite:set_position(self.final_x, self.final_y)
			unregister(self)
		else
			local x = self.transition(self.x, self.final_x, progress)
			local y = self.transition(self.y, self.final_y, progress)
			self.sprite:set_position(x, y)
		end
	end

	function rainbow.transition.move(sprite, x, y, duration, method)
		method = method or effects.linear
		local self = {
			cancel = unregister,
			tick = move,
			transition = method
		}
		self.duration = duration
		self.elapsed = 0
		self.final_x, self.final_y = x, y
		self.x, self.y = sprite:get_position()
		self.sprite = sprite
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
