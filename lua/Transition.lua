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
