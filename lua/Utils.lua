--! Rainbow Lua utilities

--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

rainbow.utils = {};

--require("Accelerometer");  -- Include the accelerometer helper

--! Dump all symbols in a table. To dump all global variables, specify _G.
--! \param story   Table to dump
--! \param offset  For internal use only
function dumptable(story, offset)
	offset = offset or "";
	local n, v;
	for n, v in pairs(story) do
		if n ~= "loaded" and n ~= "_G" then
			io.write(offset .. n .. " ");
			print(v);
			if type(v) == "table" then
				dumptable(v, offset .. "--> ");
			end
		end
	end
end

--! The C printf equivalent of Lua
function printf(s,...)
	return io.write(s:format(...));
end
