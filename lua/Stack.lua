-- Good, old fashioned stack.
--
-- Stack is a last in, first out (LIFO) data type and linear data structure. A
-- stack can store any element but only the top element is available at any
-- time.
--
-- \code
-- local Stack = require(module_path .. "Stack")
--
-- local mystack = Stack()
-- mystack:push(1)  # Stack is now { 1 }
-- mystack:push(2)  # Stack is now { 1, 2 }
-- mystack:push(3)  # Stack is now { 1, 2, 3 }
-- mystack:push(4)  # Stack is now { 1, 2, 3, 4 }
--
-- local value = mystack:top()  # 'value' is 4
--
-- mystack:pop()          # Stack is now { 1, 2, 3 }
-- value = mystack:top()  # 'value' is 3
-- mystack:pop()          # Stack is now { 1, 2 }
-- value = mystack:top()  # 'value' is 2
-- mystack:pop()          # Stack is now { 1 }
-- value = mystack:top()  # 'value' is 1
-- mystack:pop()          # Stack is now empty
-- \endcode
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local setmetatable = setmetatable

local Stack = {
	__index = nil,
	pop = nil,
	push = nil,
	top = nil
}
Stack.__index = Stack

setmetatable(Stack, {
	__call = function(Stack)
		return setmetatable({
			__count = 0,
			__store = {}
		}, Stack)
	end
})

function Stack:pop()
	if self.__count == 0 then
		return
	end
	self.__store[self.__count] = nil
	self.__count = self.__count - 1
end

function Stack:push(item)
	self.__count = self.__count + 1
	self.__store[self.__count] = item
end

function Stack:top()
	return self.__store[self.__count]
end

return Stack

--[[
function Stack.unit_test()
	local stack = Stack()
	for i = 1, 10 do
		stack:push(i)
		if stack:top() ~= i then
			print("Expected " .. i .. ", got " .. stack:top())
			error("stack:push(i) failed", 2)
		end
	end
	if stack.__count ~= 10 then
		error("stack.__count != 10", 2)
	end
	for i = 10, 1, -1 do
		stack:pop()
		if stack:pop() and stack:top() ~= stack.__count then
			print("Expected " .. stack.__count .. ", got " .. stack:top())
			error("stack:pop() failed", 2)
		end
	end
	if stack.__count ~= 0 then
		error("stack.__count != 0", 2)
	end
	stack:pop()
	if stack.__count ~= 0 then
		error("stack.__count < 0", 2)
	end
	print("Stack passed all tests")
end
--]]
