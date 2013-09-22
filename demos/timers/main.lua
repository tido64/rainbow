--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local Timer = require("Timer")

local timers = {}

local TimerResetTest = {}
TimerResetTest.__index = TimerResetTest

local TimerTimesTest = {}
TimerTimesTest.__index = TimerTimesTest

function TimerResetTest.new(times)
	local self = setmetatable({
		count = 0,
		done = false,
		reset = true,
		timer = nil,
		times = times
	}, TimerResetTest)
	local callback = function()
		self:tick()
	end
	self.timer = Timer(callback, 200, times)
	timers[#timers + 1] = self.timer
	return self
end

function TimerResetTest:tick()
	self.count = self.count + 1
	print("Check " .. self.count .. " of " .. self.times)
	if self.done then
		error("Reset test failed.")
	elseif self.count == self.times then
		if self.reset then
			self.timer:reset(1000)
			self.count = 0
			self.reset = false
		else
			self.done = true
		end
	end
end

function TimerTimesTest.new(times)
	local self = setmetatable({
		count = 0,
		done = false,
		times = times
	}, TimerTimesTest)
	local callback = function()
		self:tick()
	end
	timers[#timers + 1] = Timer(callback, 200, times)
	return self
end

function TimerTimesTest:tick()
	self.count = self.count + 1
	print("Check " .. self.count .. " of " .. self.times)
	if self.done then
		error("Times test failed.")
	elseif self.count == self.times then
		self.done = true
		TimerResetTest.new(3)
	end
end

function init()
	TimerTimesTest.new(5)
end

function update()
end
