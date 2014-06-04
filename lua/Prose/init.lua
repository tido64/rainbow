-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Parser = require((...) .. "/Parser")

local collectgarbage = collectgarbage
local pairs = pairs
local setmetatable = setmetatable

local scenegraph = rainbow.scenegraph

local function emptytable(t)
	for k,v in pairs(t) do
		t[k] = nil
	end
end

local Prose = {
	__gc = function(self)
		if rainbow.scenegraph == nil then
			-- We're shutting down, all resources are released.
			return
		end
		if self.resources then
			emptytable(self.resources)
			self.resources = nil
		end
		if self.objects then
			emptytable(self.objects)
			self.objects = nil
		end
		if self.node then
			scenegraph:remove(self.node)
			self.node = nil
		end
	end,
	__index = nil,
	from_table = nil
}

Prose.__index = setmetatable(Prose, {
	__call = function()
		return setmetatable({
			node = scenegraph:add_node(),
			objects = false,
			resources = false
		}, Prose)
	end
})

function Prose.from_table(t)
	if type(t) ~= "table" or t.version < Parser.version then
		return nil
	end
	collectgarbage("stop")
	local prose = Prose()
	prose.resources = Parser.createresources(t.resources)
	prose.objects = Parser.createnodes(prose.node, prose.resources, t.nodes)
	collectgarbage("restart")
	return prose
end

return Prose
