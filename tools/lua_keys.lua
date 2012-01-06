#!/usr/bin/lua

function parse_key(str, prefix)
	local start_pos, len = string.find(str, prefix)
	if not start_pos then
		return nil
	end
	len = len - 1
	local end_pos = string.find(str, "%s", start_pos + len)
	local enum = nil
	if end_pos then
		local comma = string.find(str, ",", start_pos + len)
		if comma and comma < end_pos then
			end_pos = nil;
		else
			enum, len = string.find(str, ",", end_pos)
			if enum then
				enum = tonumber(string.sub(str, end_pos + 3, enum - 1))
			end
		end
	end
	if not end_pos then
		end_pos = string.find(str, ",", start_pos + len)
		if not end_pos then
			return nil
		end
	end
	return string.sub(str, start_pos, end_pos - 1), enum
end

io.input("src/Input/Key.h")
enum = 0
keys_lua = "rainbow.keyboard = {\n"
for line in io.lines() do
	if parse_key(line, "mod_") then break end
	local key, e = parse_key(line, "key_")
	if not key then
		key, e = parse_key(line, "numpad_")
	end
	if key then
		if e then
			enum = e
		end
		keys_lua = keys_lua  .. "	" .. key .. " = " .. enum .. ",\n"
		enum = enum + 1
	end
end
io.output("lua/Keyboard.lua")
io.write("--! Raw keyboard codes.\n--!\n");
io.write("--! This file was auto-generated with lua_keys.lua.\n");
io.write(os.date("--! Copyright %Y Bifrost Games. All rights reserved.\n\n"));
io.write(string.sub(keys_lua, 0, -3) .. "\n};\n")
