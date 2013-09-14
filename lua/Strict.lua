-- Strict.lua - Prevents accidental global variables
--
-- Adapted from Niklas Frykholm's solution for enforcing local declaration:
--   http://lua-users.org/wiki/DetectingUndefinedVariables
--
-- Copyright (c) 2013 Tommy Nguyen
--
-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
-- copies of the Software, and to permit persons to whom the Software is
-- furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
-- OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
-- THE SOFTWARE.

local rawset = rawset
local type = type

local mt = getmetatable(_G) or { __newindex = nil }
mt.__newindex = function(t, k, v)
	if type(v) ~= "function" and (k:len() < 3 or k:sub(1, 2) ~= "g_") then
		local mt = getmetatable(_G)
		mt.__newindex = rawset
		setmetatable(_G, mt)
		return error("Cannot declare global '" .. k .. "' without 'g_' as a prefix" , 2)
	end
	rawset(t, k, v)
end
setmetatable(_G, mt)
