--! Scheduler class makes sure objects are notifed at specified intervals.

--! All objects must implement the following function:
--!
--! - tick()
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

Scheduler = {};
Scheduler.__index = Scheduler;

function Scheduler.new(secs)
	local a = {};
	setmetatable(a, Scheduler);
	a.count = 0;
	a.subscribers = {};
	a.ticker = "0";
	a.time = 0;
	a.timeout = secs;
	return a;
end

--! Add an object that needs to be notified.
function Scheduler:add(obj)
	self.count = self.count + 1;
	self.subscribers[self.count] = obj;
end

--! Remove object from scheduler.
function Scheduler:remove(obj)
	local sub = 0;
	for i = 1, self.count do
		if self.subscribers[i] == obj then
			sub = i;
			break;
		end
	end
	if sub == 0 then
		return;
	end
	self.subscribers[sub] = self.subscribers[self.count];
	self.subscribers[self.count] = nil;
	self.count = self.count - 1;
end

--! Remove all objects from scheduler.
function Scheduler:remove_all()
	for i = 1, self.count do
		self.subscribers[i] = nil;
	end
	self.count = 0;
end

--! Notify all objects at specified interval.
function Scheduler:update()
	local time = os.date("%Y%m%d%H%M%S");
	if time > self.ticker then
		self.ticker = time;
		self.time = self.time + 1;
		if self.time > self.timeout then
			for i = 1, self.count do
				self.subscribers[i]:tick();
			end
			self.time = 1;
		end
	end
end
