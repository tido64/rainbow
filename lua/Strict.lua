-- Strict.lua - Prevents accidental global variables
--
-- Adapted from Niklas Frykholm's solution for enforcing local declaration:
--   http://lua-users.org/wiki/DetectingUndefinedVariables
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
