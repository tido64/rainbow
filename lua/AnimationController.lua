-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local scenegraph = rainbow.scenegraph
local setmetatable = setmetatable
local table = table

local function notify(listener, event)
	return listener and listener[event] and listener[event](listener)
end

local function stop(self)
	if self.node then
		scenegraph:remove(self.node)
		self.node = nil
	end
	if self.current then
		self.current:stop()
		self.current:set_listener(nil)
		self.current = nil
	end
end

local function play(self, name, loop)
	stop(self)
	self.current = self.animations[name]
	self.current:play()
	self.current:set_listener(self)
	self.node = scenegraph:add_animation(self.parent, self.current)
end

local AnimationController = {
	__gc = nil,
	__index = nil,
	add_animation = nil,
	set_animation = nil,
	set_listener = nil,
	on_animation_start = nil,
	on_animation_end = nil,
	on_animation_complete = nil
}

AnimationController.__index = setmetatable(AnimationController, {
	__call = function(AnimationController, animations)
		return setmetatable({
			animations = animations,
			current = nil,
			listener = nil,
			node = nil,
			parent = nil,
			queue = {}
		}, AnimationController)
	end
})

function AnimationController:__gc()
	stop(self)
end

function AnimationController:add_animation(track, name, loop, delay)
	local n = #self.queue
	if n == 0 then
		return self:set_animation(track, name, loop)
	end
	self.queue[n + 1] = { name, loop }
end

function AnimationController:set_animation(_, name, loop)
	self.queue = {{ name, loop }}
	play(self, name, loop)
end

function AnimationController:set_listener(listener)
	self.listener = listener
end

function AnimationController:on_animation_start()
	return notify(self.listener, "on_animation_start")
end

function AnimationController:on_animation_end()
	if #self.queue == 1 then
		local name, loop = 1, 2  -- indices
		if self.queue[1][name] ~= self.current then
			return
		elseif not self.queue[1][loop] then
			stop(self)
			table.remove(self.queue)
			return notify(self.listener, "on_animation_end")
		end
		self.current:play()
	end
	return self:on_animation_complete()
end

function AnimationController:on_animation_complete()
	local name, loop = 1, 2  -- indices
	if #self.queue == 1 then
		return not self.queue[1][loop]
		    and self:on_animation_end()
		     or notify(self.listener, "on_animation_complete")
	end
	table.remove(self.queue, 1)
	local entry = self.queue[1]
	play(self, entry[name], entry[loop])
end

return AnimationController
