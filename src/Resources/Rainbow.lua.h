// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef RESOURCES_RAINBOW_LUA_H_
#define RESOURCES_RAINBOW_LUA_H_

const char Rainbow_lua[] = R"lua(

local assert = assert
local getmetatable = getmetatable
local table_remove = table.remove

local __module_count = 1
local __modules = { nil }

function __update(dt)
	for i = 1, __module_count do
		__modules[i](dt)
	end
	update(dt)
end

local function get(module)
	local module_update = getmetatable(module).__update
	assert(module_update, "Invalid module: Missing __update() function")
	return module_update
end

rainbow.module = {
	register = function(module)
		local module_update = get(module)
		for i = 1, __module_count do
			if __modules[i] == module_update then
				return module
			end
		end
		__module_count = __module_count + 1
		__modules[__module_count] = module_update
		return module
	end,
	unregister = function(module)
		local module_update = get(module)
		for i = 1, __module_count do
			if __modules[i] == module_update then
				table_remove(__modules, i)
				__module_count = __module_count - 1
				return true
			end
		end
	end
}

local Input = rainbow.input

local __listener_count = 0
local __listeners = setmetatable({ [0] = {} }, { __mode = "v" })

local function for_each_listener(f, ...)
	for i = 1, __listener_count do
		local listener = __listeners[i]
		if listener then
			f(listener, ...)
		end
	end
end

function Input.subscribe(obj)
	__listener_count = __listener_count + 1
	__listeners[__listener_count] = obj
end

function Input.unsubscribe(obj)
	for i = 1, __listener_count do
		if __listeners[i] == obj then
			__listeners[i] = false
			break
		end
	end
end

function Input.unsubscribe_all()
	__listeners[1] = nil
	__listener_count = 0
end

function Input.key_down(keys)
	for_each_listener(function(l, key, mod)
		l:key_down(key, mod)
	end, keys[1], keys[2])
end

function Input.key_up(keys)
	for_each_listener(function(l, key, mod)
		l:key_up(key, mod)
	end, keys[1], keys[2])
end

function Input.pointer_began(pointers)
	for_each_listener(function(l, pointers)
		l:pointer_began(pointers)
	end, pointers)
end

function Input.pointer_canceled()
	for_each_listener(function(l)
		l:pointer_canceled()
	end)
end

function Input.pointer_ended(pointers)
	for_each_listener(function(l, pointers)
		l:pointer_ended(pointers)
	end, pointers)
end

function Input.pointer_moved(pointers)
	for_each_listener(function(l, pointers)
		l:pointer_moved(pointers)
	end, pointers)
end

__modules[1] = function()
	if not __listeners[0] then
		__listeners[0] = {}
		local i = 0
		for_each_listener(function(l)
			i = i + 1
			__listeners[i] = l
		end)
		__listener_count = i
	end
	local events = Input.__events
	for i = 1, events[0] do
		local e = events[i]
		Input[e[1]](e[2])
	end
end
)lua";

#endif
