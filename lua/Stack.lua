--! Good, old fashioned stack.
--!
--! Stack is a last in, first out (LIFO) data type and linear data structure.
--! A stack can store any element but only the top element is available at any
--! time. Two fundamental operations characterise a stack:
--!
--! - \c push(item) adds \c item on top of the stack.
--! - \c pop() removes the top element.
--!
--! The top element is accessed by \c :top().
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

Stack = {}
Stack.__index = Stack

function Stack.new(secs)
	local self = {}
	setmetatable(self, Stack)
	self.count = 0
	self.store = {}
	return self
end

function Stack:pop()
	if self.count == 0 then
		return
	end
	self.store[self.count] = nil
	self.count = self.count - 1
end

function Stack:push(item)
	self.count = self.count + 1
	self.store[self.count] = item
end

function Stack:top()
	return self.store[self.count]
end

--[[
function Stack.unit_test()
	local stack = Stack.new()
	for i = 1, 10 do
		stack:push(i)
		if stack:top() ~= i then
			print("Expected " .. i .. ", got " .. stack:top())
			error("stack:push(i) failed", 2)
		end
	end
	if stack.count ~= 10 then
		error("stack.count != 10", 2)
	end
	for i = 10, 1, -1 do
		stack:pop()
		if stack:pop() and stack:top() ~= stack.count then
			print("Expected " .. stack.count .. ", got " .. stack:top())
			error("stack:pop() failed", 2)
		end
	end
	if stack.count ~= 0 then
		error("stack.count != 0", 2)
	end
	stack:pop()
	if stack.count ~= 0 then
		error("stack.count < 0", 2)
	end
	print("Stack passed all tests")
end
--]]
