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

__rainbow_input_listeners = {};
__rainbow_input_listener_count = 0;

--! Add an object as input listener.
function rainbow.input.subscribe(obj)
	__rainbow_input_listener_count = __rainbow_input_listener_count + 1;
	__rainbow_input_listeners[__rainbow_input_listener_count] = obj;
end

--! Remove a listener.
function rainbow.input.unsubscribe(obj)
	local listener = 0;
	for i = 1, __rainbow_input_listener_count do
		if __rainbow_input_listeners[i] == obj then
			listener = i;
			break;
		end
	end
	if listener == 0 then
		return;
	end
	__rainbow_input_listeners[listener] = __rainbow_input_listeners[__rainbow_input_listener_count];
	__rainbow_input_listeners[__rainbow_input_listener_count] = nil;
	__rainbow_input_listener_count = __rainbow_input_listener_count - 1;
end

--! Remove all listeners.
function rainbow.input.unsubscribe_all()
	for i = 1, __rainbow_input_listener_count do
		__rainbow_input_listeners[i] = nil;
	end
	__rainbow_input_listener_count = 0;
end

--! Notify all listeners of a touch begun event.
function rainbow.input.touch_began()
	for i = 1, __rainbow_input_listener_count do
		__rainbow_input_listeners[i]:touch_began();
	end
end

--! Notify all listeners of a touch canceled event.
function rainbow.input.touch_canceled()
	for i = 1, __rainbow_input_listener_count do
		__rainbow_input_listeners[i]:touch_canceled();
	end
end

--! Notify all listeners of a touch ended event.
function rainbow.input.touch_ended()
	for i = 1, __rainbow_input_listener_count do
		__rainbow_input_listeners[i]:touch_ended();
	end
end

--! Notify all listeners of a touch moved event.
function rainbow.input.touch_moved()
	for i = 1, __rainbow_input_listener_count do
		__rainbow_input_listeners[i]:touch_moved();
	end
end
