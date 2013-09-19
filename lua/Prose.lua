local module_path = ...
local Parser = require(module_path .. "/Parser")
local Prose = require(module_path .. "/Prose")

rainbow.prose = rainbow.prose or {}
function rainbow.prose.from_table(t)
	if type(t) ~= "table" then
		return nil
	end
	collectgarbage("stop")
	local prose = Prose()
	prose.resources = Parser.createresources(t.resources)
	prose.objects = Parser.createnodes(prose.node, prose.resources, t.nodes)
	collectgarbage("restart")
	return prose
end
