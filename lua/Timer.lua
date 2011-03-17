-- A time event trigger class.

-- Add objects that need to be called at a specific interval.
--
-- Copyright 2011 Bifrost Games. All rights reserved.
-- \author Tommy Nguyen

Timer = {};
Timer.__index = Timer;

function Timer.new(secs)
	local a = {};
	setmetatable(a, Timer);
	a.count = 0;
	a.subscribers = {};
	a.ticker = "0";
	a.time = 0;
	a.timeout = secs;
	return a;
end

function Timer:add(obj)
	self.count = self.count + 1;
	self.subscribers[self.count] = obj;
end

function Timer:update()
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
