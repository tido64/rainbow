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

rainbow.input._listeners = {}
rainbow.input._count = 0
rainbow.input._down = false

--! Add an object as input listener.
function rainbow.input.subscribe(obj)
	local input = rainbow.input
	input._count = input._count + 1
	input._listeners[input._count] = obj
end

--! Remove a listener.
function rainbow.input.unsubscribe(obj)
	local input = rainbow.input
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		if listeners[i] == obj then
			listeners[i] = listeners[input._count]
			listeners[input._count] = nil
			input._count = input._count - 1
			break
		end
	end
end

--! Remove all listeners.
function rainbow.input.unsubscribe_all()
	local input = rainbow.input
	local listeners = input._listeners
	for i = 1, input._count do
		listeners[i] = nil
	end
	input._count = 0
end

--! Notify all listeners of a key down event.
function rainbow.input.key_down(key, mod)
	print("Key down: " .. mod .. "+" .. key)

	local input = rainbow.input
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:key_down(key, mod)
	end
end

--! Notify all listeners of a key up event.
function rainbow.input.key_up(key, mod)
	print("Key up: " .. mod .. "+" .. key)

	local input = rainbow.input
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:key_up(key, mod)
	end
end

--! Notify all listeners of a touch began event.
function rainbow.input.touch_began(touches)
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " began at (" .. t.x .. "," .. t.y .. ")")
	end

	local input = rainbow.input
	input._down = true
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:touch_began(touches)
	end
end

--! Notify all listeners of a touch canceled event.
function rainbow.input.touch_canceled()
	print("Touch events canceled")

	local input = rainbow.input
	input._down = false
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:touch_canceled()
	end
end

--! Notify all listeners of a touch ended event.
function rainbow.input.touch_ended(touches)
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " ended at (" .. t.x .. "," .. t.y .. ")")
	end

	local input = rainbow.input
	input._down = false
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:touch_ended(touches)
	end
end

--! Notify all listeners of a touch moved event.
function rainbow.input.touch_moved(touches)
	local input = rainbow.input
	if input._down then
		for h,t in pairs(touches) do
			print("Touch event #" .. h .. " moved to (" .. t.x .. "," .. t.y .. ")")
		end
	end
	if input._count == 0 then
		return
	end

	local listeners = input._listeners
	for i = input._count, 1 do
		listeners[i]:touch_moved(touches)
	end
end
