--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local table_remove = table.remove

local __count = 0
local __timers = {}

--! Checks whether it is time to call the callback. If the number of called
--! times is up, the timer is effectively canceled.
local function test(self, dt)
	self.__delay = self.__delay - dt
	if self.__delay <= 0 then
		if self.times then
			if self.__times == self.times then
				self:cancel()
				return
			end
			self.__times = self.__times + 1
		end
		self.__delay = self.delay
		self.callback()
	end
end

local Timer = {
	__index = nil,
	cancel = nil,
	clear = nil,
	reset = nil
}
Timer.__index = Timer

--! Clears all timers.
function Timer.clear()
	__count = 0
	__timers = {}
end

setmetatable(Timer, {
	--! Creates a timer.
	--! \param callback  The function to call on time-out.
	--! \param delay     Time in milliseconds to delay call.
	--! \param times     Number of times to call. Infinite if omitted.
	__call = function(Timer, callback, delay, times)
		assert(type(callback) == "function", "Invalid callback")
		local self = setmetatable({
			__delay = delay,
			__times = 0,
			callback = callback,
			delay = delay,
			times = times and times > 0 and times
		}, Timer)
		__count = __count + 1
		__timers[__count] = self
		return self
	end,
	--! Iterate over and test all active timers.
	__update = function(dt)
		for i = __count, 1, -1 do
			test(__timers[i], dt)
		end
	end
})

--! Cancels timer.
function Timer:cancel()
	for i = 1, __count do
		if __timers[i] == self then
			table_remove(__timers, i)
			__count = __count - 1
			break
		end
	end
end

--! Resets a timer, restoring its initial state.
--! \param delay  Optional new delay.
function Timer:reset(delay)
	self.delay = delay or self.delay
	self.__delay = self.delay
	self.__times = 0
	for i = 1, __count do
		if __timers[i] == self then
			return
		end
	end
	__count = __count + 1
	__timers[__count] = self
end

return rainbow.module.register(Timer)
