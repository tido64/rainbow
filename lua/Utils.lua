--! Rainbow Lua utilities

--! Copyright 2011 Bifrost Games. All rights reserved.
--! \author Tommy Nguyen

rainbow.utils = {};

--! The printf equivalent of C
function printf(s,...)
	return io.write(s:format(...))
end

--! Dump all symbols in a table. To dump all global variables, specify _G.
--! \param story   Table to dump
--! \param offset  For internal use only
function rainbow.utils.dump_table(story, offset)
	offset = offset or "";
	local n, v;
	for n, v in pairs(story) do
		if n ~= "loaded" and n ~= "_G" then
			io.write(offset .. n .. " ");
			print(v);
			if type(v) == "table" then
				rainbow.utils.dump_table(v, offset .. "--> ");
			end
		end
	end
end
