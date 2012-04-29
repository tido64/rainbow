--! Scheduler class makes sure objects are notifed at specified intervals.
--!
--! All objects must implement the following function:
--!
--! - tick()
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

Scheduler = {}
Scheduler.__index = Scheduler

--! Create a scheduler that ticks every \p t seconds or milliseconds. If \p t
--! is less than 300, it is interpreted as seconds. Otherwise, it's
--! milliseconds.
function Scheduler.new(t)
	if t < 300 then
		t = t * 1000
	end
	local self = {}
	setmetatable(self, Scheduler)
	self.count = 0
	self.elapsed = 0
	self.subscribers = {}
	self.timeout = t
	return self
end

--! Add an object that needs to be notified.
function Scheduler:add(obj)
	if self.count == 0 then
		self.elapsed = 0
	end
	self.count = self.count + 1
	self.subscribers[self.count] = obj
end

--! Remove object from scheduler.
function Scheduler:remove(obj)
	for i = 1, self.count do
		if self.subscribers[i] == obj then
			self.subscribers[i] = self.subscribers[self.count]
			self.subscribers[self.count] = nil
			self.count = self.count - 1
			break
		end
	end
end

--! Remove all objects from scheduler.
function Scheduler:remove_all()
	for i = 1, self.count do
		self.subscribers[i] = nil
	end
	self.count = 0
end

--! Notify all objects at specified interval.
function Scheduler:update(dt)
	if self.count == 0 then
		return
	end

	self.elapsed = self.elapsed + dt
	while self.elapsed >= self.timeout do
		for i = self.count, 1, -1 do
			self.subscribers[i]:tick()
		end
		self.elapsed = self.elapsed - self.timeout
	end
end
