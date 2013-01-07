--! Rainbow Lua utilities
--!
--! Copyright 2011-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

--! The C printf equivalent of Lua
function printf(s,...)
	return io.write(s:format(...));
end

--! Override Lua's setmetatable to support nested metatables.
local lua_setmetatable = setmetatable
function setmetatable(t, mt)
	local _mt = getmetatable(t)
	if _mt then
		local _innermost = mt
		local _inner = getmetatable(_innermost)
		-- Find the innermost metatable
		while _inner and _inner ~= _innermost and _inner ~= _mt do
			_innermost = _inner
			_inner = getmetatable(mt)
		end
		if _inner ~= _mt then
			lua_setmetatable(_innermost, _mt)
		end
	end
	lua_setmetatable(t, mt)
end
