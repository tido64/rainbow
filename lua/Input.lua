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
--! Copyright 2011 Bifrost Games. All rights reserved.
--! \author Tommy Nguyen

require("Keyboard");  -- Table of raw keycodes

_rainbow_input_listeners = {};
_rainbow_input_listener_count = 0;

--! Add an object as input listener.
function rainbow.input.subscribe(obj)
	_rainbow_input_listener_count = _rainbow_input_listener_count + 1;
	_rainbow_input_listeners[_rainbow_input_listener_count] = obj;
end

--! Remove a listener.
function rainbow.input.unsubscribe(obj)
	local listener = 0;
	for i = 1, _rainbow_input_listener_count do
		if _rainbow_input_listeners[i] == obj then
			listener = i;
			break;
		end
	end
	if listener == 0 then
		return;
	end
	_rainbow_input_listeners[listener] = _rainbow_input_listeners[_rainbow_input_listener_count];
	_rainbow_input_listeners[_rainbow_input_listener_count] = nil;
	_rainbow_input_listener_count = _rainbow_input_listener_count - 1;
end

--! Remove all listeners.
function rainbow.input.unsubscribe_all()
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i] = nil;
	end
	_rainbow_input_listener_count = 0;
end

--! Notify all listeners of a key down event.
function rainbow.input.key_down(key, mod)
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:key_down(key);
	end
	print("Key down: " .. mod .. "+" .. key);
end

--! Notify all listeners of a key up event.
function rainbow.input.key_up(key, mod)
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:key_up(key);
	end
	print("Key up: " .. mod .. "+" .. key);
end

--! Notify all listeners of a touch began event.
function rainbow.input.touch_began(touches)
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_began(touches);
	end
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " began at (" .. t.x .. "," .. t.y .. ")");
	end
end

--! Notify all listeners of a touch canceled event.
function rainbow.input.touch_canceled()
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_canceled();
	end
	--print("Touch events canceled");
end

--! Notify all listeners of a touch ended event.
function rainbow.input.touch_ended(touches)
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_ended(touches);
	end
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " ended at (" .. t.x .. "," .. t.y .. ")");
	end
end

--! Notify all listeners of a touch moved event.
function rainbow.input.touch_moved(touches)
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_moved(touches);
	end
	for h,t in pairs(touches) do
		print("Touch event #" .. h .. " moved to (" .. t.x .. "," .. t.y .. ")");
	end
end
