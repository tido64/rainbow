-- Rainbow Lua utilities

-- Copyright 2011 Bifrost Games. All rights reserved.
-- \author Tommy Nguyen

-- Dumps all symbols in a table. To dump all global variables, specify _G.
-- \param offset  For internal use only
-- \param story   Table to dump
function rainbow.dump_table(story, offset)
	offset = offset or "";
	local n, v;
	for n, v in pairs(story) do
		if n ~= "loaded" and n ~= "_G" then
			io.write(offset .. n .. " ");
			print(v);
			if type(v) == "table" then
				rainbow.dump_table(v, offset .. "--> ");
			end
		end
	end
end
