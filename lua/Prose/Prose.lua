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
			rainbow.scenegraph:remove(self.node)
			self.node = nil
		end
	end,
	__index = false
}

Prose.__index = Prose

local function create()
	local self = setmetatable({
		node = rainbow.scenegraph:add_node(),
		objects = false,
		resources = false
	}, Prose)
	return self
end

return setmetatable(Prose, { __call = create })
