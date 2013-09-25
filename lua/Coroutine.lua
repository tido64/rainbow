--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local coroutine_create = coroutine.create
local coroutine_resume = coroutine.resume
local coroutine_yield = coroutine.yield
local table_remove = table.remove

local __coroutines = {}
local __count = 0
local __index = -1

local Coroutine = {
	__index = nil,
	start = nil,
	wait = nil
}

Coroutine.__index = setmetatable(Coroutine, {
	__update = function(dt)
		for i = __count, 1, -1 do
			if not coroutine_resume(__coroutines[i], dt) then
				table_remove(__coroutines, i)
				__count = __count - 1
			end
		end
	end
})

--! Creates and starts a coroutine executing \p fn.
function Coroutine.start(fn)
	__count = __count + 1
	local co = coroutine_create(fn)
	__coroutines[__count] = co
	return coroutine_resume(co, dt)
end

--! Blocks execution for \p t milliseconds.
function Coroutine.wait(t)
	t = t or 1
	t = t > 0 and t or 1
	local a = 0
	while a < t do
		a = a + coroutine_yield()
	end
end

return rainbow.module.register(Coroutine)
