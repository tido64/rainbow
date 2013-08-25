--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local __coroutines = {}
local __count = 0
local __index = -1

local Coroutine = {}
Coroutine.__index = Coroutine

local function update(dt)
	for i = __count, 1, -1 do
		if not coroutine.resume(__coroutines[i], dt) then
			table.remove(__coroutines, i)
			__count = __count - 1
			if __count == 0 then
				table.remove(__rainbow_modules, __index)
			end
		end
	end
end

--! Creates and starts a coroutine executing \p fn.
function Coroutine.start(fn)
	if __count == 0 then
		__index = #__rainbow_modules + 1
		__rainbow_modules[__index] = update
	end
	__count = __count + 1
	local co = coroutine.create(fn)
	__coroutines[__count] = co
	return coroutine.resume(co, dt)
end

--! Blocks execution for \p t milliseconds.
function Coroutine.wait(t)
	t = t or 1
	t = t > 0 and t or 1
	local a = 0
	while a < t do
		a = a + coroutine.yield()
	end
end

rainbow.coroutine = Coroutine
