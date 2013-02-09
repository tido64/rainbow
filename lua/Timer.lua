--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local __count = 0
local __list  = {}

local Timer = {}
Timer.__index = Timer
rainbow.timer = Timer

--! Clear all timers.
function Timer.clear()
	__count = 0
	__list = {}
end

--! Create a timer.
--! \param callback  The callback function or object implementing \c tick().
--! \param delay     Time in milliseconds to delay call.
--! \param times     Number of times to call. Infinite if omitted.
local function create(Timer, callback, delay, times)
	local self = {
		__id = __count + 1,
		__delay = delay,
		__times = 0,
		callback = callback,
		delay = delay,
		times = times and times > 0 and times
	}
	setmetatable(self, Timer)
	__count = self.__id
	__list[__count] = self
	return self
end
setmetatable(Timer, { __call = create })

--! Cancel timer.
function Timer:cancel()
	for i = self.__id + 1, __count do
		local j = i - 1
		__list[i].__id = j
		__list[j] = __list[i]
	end
	__list[__count] = nil
	__count = __count - 1
	self.__id = nil
end

--! Reset a timer. Restores it to its initial state.
--! \param delay  Optional new delay.
function Timer:reset(delay)
	if not self.__id then
		self.__id = __count + 1
		__count = self.__id
		__list[__count] = self
	end
	self.delay = delay or self.delay
	self.__delay = self.delay
	self.__times = 0
end

--! Check whether it is time to call the callback. If the number of called
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
		if type(self.callback) == "table" then
			self.callback:tick()
		else
			self.callback()
		end
	end
end

--! Iterate over and test all active timers.
local function update(dt)
	for i = __count, 1, -1 do
		test(__list[i], dt)
	end
end
__rainbow_modules[#__rainbow_modules + 1] = update
