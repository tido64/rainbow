--! Rainbow Lua input subscription class.
--!
--! Lets objects subscribe to and be notified of input events. All listeners
--! must implement the following methods:
--!
--! - key_down()
--! - key_up()
--! - touch_began()
--! - touch_canceled()
--! - touch_ended()
--! - touch_moved()
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require("Keyboard")  -- Table of raw keycodes

local __count     = 0
local __down      = false
local __listeners = {}

--! Add an object as input listener.
function rainbow.input.subscribe(obj)
	__count = __count + 1
	__listeners[__count] = obj
end

--! Remove a listener.
function rainbow.input.unsubscribe(obj)
	for i = __count, 1, -1 do
		if __listeners[i] == obj then
			__listeners[i] = __listeners[__count]
			__listeners[__count] = nil
			__count = __count - 1
			break
		end
	end
end

--! Remove all listeners.
function rainbow.input.unsubscribe_all()
	for i = 1, __count do
		__listeners[i] = nil
	end
	__count = 0
end

--! Notify all listeners of a key down event.
function rainbow.input.key_down(key, mod)
	print("Key down: " .. mod .. "+" .. key)

	for i = __count, 1, -1 do
		__listeners[i]:key_down(key, mod)
	end
end

--! Notify all listeners of a key up event.
function rainbow.input.key_up(key, mod)
	print("Key up: " .. mod .. "+" .. key)

	for i = __count, 1, -1 do
		__listeners[i]:key_up(key, mod)
	end
end

--! Notify all listeners of a touch began event.
function rainbow.input.touch_began(touches)
	--for h,t in pairs(touches) do
	--	print("Touch event #" .. h .. " began at (" .. t.x .. "," .. t.y .. ")")
	--end

	__down = true
	for i = __count, 1, -1 do
		__listeners[i]:touch_began(touches)
	end
end

--! Notify all listeners of a touch canceled event.
function rainbow.input.touch_canceled()
	--print("Touch events canceled")

	__down = false
	for i = __count, 1, -1 do
		__listeners[i]:touch_canceled()
	end
end

--! Notify all listeners of a touch ended event.
function rainbow.input.touch_ended(touches)
	--for h,t in pairs(touches) do
	--	print("Touch event #" .. h .. " ended at (" .. t.x .. "," .. t.y .. ")")
	--end

	__down = false
	for i = __count, 1, -1 do
		__listeners[i]:touch_ended(touches)
	end
end

--! Notify all listeners of a touch moved event.
function rainbow.input.touch_moved(touches)
	--if __down then
	--	for h,t in pairs(touches) do
	--		print("Touch event #" .. h .. " moved to (" .. t.x .. "," .. t.y .. ")")
	--	end
	--end

	for i = __count, 1, -1 do
		__listeners[i]:touch_moved(touches)
	end
end
