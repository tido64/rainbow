--! Rainbow Lua input subscription class.
--!
--! Input events are only sent to objects that subscribe to them. Such objects
--! are called event listeners. A listener can be implemented as follows.
--!
--! \code
--! InputListener = {}
--! InputListener.__index = InputListener
--!
--! function InputListener:key_down(key, modifiers) end
--! function InputListener:key_up(key, modifiers) end
--! function InputListener:touch_ended(touches) end
--! function InputListener:touch_canceled() end
--! function InputListener:touch_moved(touches) end
--!
--! # Create our listener and let it subscribe to input events.
--! local mylistener = setmetatable({}, InputListener)
--! rainbow.input.subscribe(mylistener)
--!
--! # We're only interested in touch began events.
--! function mylistener:touch_began(touches)
--! 	for hash,touch in pairs(touches) do
--! 		# Handle event here
--! 	end
--! end
--! \endcode
--!
--! As seen in the example, the easiest way is to define an \c InputListener
--! and inherit from it, then define the functions that are needed. The
--! important point here is that all event handlers must be implemented even if
--! they'll do nothing.
--!
--! For touch events, a table of events are sent with each notification. It is
--! iterated as above. The \c hash value uniquely identifies a touch (or mouse
--! button) for the duration of it touching the screen (or mouse button being
--! held). Touch (or mouse click) location is stored in \c touch:
--!
--! \code
--! touch.x          # For the x-coordinate.
--! touch.y          # For the y-coordinate.
--! touch.timestamp  # For the relative time at which the event occurred.
--! \endcode
--!
--! Copyright 2011-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

--require("Keyboard")  -- Table of raw keycodes

local Input = rainbow.input

local __count     = 0
local __down      = false
local __listeners = {}

--! Add an object as input listener.
function Input.subscribe(obj)
	__count = __count + 1
	__listeners[__count] = obj
end

--! Remove a listener.
function Input.unsubscribe(obj)
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
function Input.unsubscribe_all()
	for i = 1, __count do
		__listeners[i] = nil
	end
	__count = 0
end

--! Notify all listeners of a key down event.
function Input.key_down(keys)
	local key, mod = keys[1], keys[2]
	print("Key down: " .. mod .. "+" .. key)

	for i = __count, 1, -1 do
		__listeners[i]:key_down(key, mod)
	end
end

--! Notify all listeners of a key up event.
function Input.key_up(keys)
	local key, mod = keys[1], keys[2]
	print("Key up: " .. mod .. "+" .. key)

	for i = __count, 1, -1 do
		__listeners[i]:key_up(key, mod)
	end
end

--! Notify all listeners of a touch began event.
function Input.touch_began(touches)
	--[[
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " began at (" .. t.x .. "," .. t.y .. ")")
	end
	--]]

	__down = true
	for i = __count, 1, -1 do
		__listeners[i]:touch_began(touches)
	end
end

--! Notify all listeners of a touch canceled event.
function Input.touch_canceled()
	--print("Touch events canceled")

	__down = false
	for i = __count, 1, -1 do
		__listeners[i]:touch_canceled()
	end
end

--! Notify all listeners of a touch ended event.
function Input.touch_ended(touches)
	--[[
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " ended at (" .. t.x .. "," .. t.y .. ")")
	end
	--]]

	__down = false
	for i = __count, 1, -1 do
		__listeners[i]:touch_ended(touches)
	end
end

--! Notify all listeners of a touch moved event.
function Input.touch_moved(touches)
	--[[
	if __down then
		for h,t in pairs(touches) do
			print("Touch event #" .. h .. " moved to (" .. t.x .. "," .. t.y .. ")")
		end
	end
	--]]

	for i = __count, 1, -1 do
		__listeners[i]:touch_moved(touches)
	end
end

local function fireevents()
	local events = Input.__events;
	for i = 1, events[0] do
		local e = events[i]
		Input[e[1]](e[2])
	end
end
__rainbow_modules[#__rainbow_modules + 1] = fireevents
