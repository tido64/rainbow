--! Rainbow Lua input subscription class.

--! Lets objects subscribe to and be notified of input events. All listeners
--! must implement the following methods:
--!
--! - touch_began()
--! - touch_canceled()
--! - touch_ended()
--! - touch_moved()
--!
--! Copyright 2011 Bifrost Games. All rights reserved.
--! \author Tommy Nguyen

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

--! Notify all listeners of a touch began event.
function rainbow.input.touch_began()
	local t = rainbow.input.touches;
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_began(t);
	end
	--for h,t in pairs(t) do
		--print("Touch event #" .. h .. " began at (" .. t.x .. "," .. t.y .. ")");
	--end
end

--! Notify all listeners of a touch canceled event.
function rainbow.input.touch_canceled()
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_canceled();
	end
	--print("Touch events canceled");
end

--! Notify all listeners of a touch ended event.
function rainbow.input.touch_ended()
	local t = rainbow.input.touches;
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_ended();
	end
	--for h,t in pairs(t) do
		--print("Touch event #" .. h .. " ended at (" .. t.x .. "," .. t.y .. ")");
	--end
end

--! Notify all listeners of a touch moved event.
function rainbow.input.touch_moved()
	local t = rainbow.input.touches;
	for i = 1, _rainbow_input_listener_count do
		_rainbow_input_listeners[i]:touch_moved(t);
	end
	--for h,t in pairs(t) do
		--print("Touch event #" .. h .. " moved to (" .. t.x .. "," .. t.y .. ")");
	--end
end
