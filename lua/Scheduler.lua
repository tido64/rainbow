--! Scheduler class makes sure objects are notifed at specified intervals.

--! Copyright 2011 Bifrost Games. All rights reserved.
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

--! Notify all objects at specified intervals.
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
