--! Simple parallax controller.
--!
--! \code
--! local Parallax = require(module_path .. "Parallax")
--!
--! local layers = {
--! 	-- A layer is defined by a batch and its velocity on the x- and y-axis
--! 	{ batch1, 0.5, 0 }  -- background
--! 	{ batch2, 0.8, 0 }  -- midground
--! 	{ batch3, 1.0, 0 }  -- foreground
--! }
--! local pax = Parallax(layers)
--! pax:move(0.7 * dt)
--! \endcode
--!
--! Limitations:
--!   - Does not handle loops.
--!   - There's a 1:1 mapping between batches and layers even though, logically,
--!     batches can be on the same layer.
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local module_path = (...):match("(.*[./\\])[^./\\]+") or ""
local setmetatable = setmetatable
local table_unpack = table.unpack or unpack

local F = require(module_path .. "Functional")
local scenegraph = rainbow.scenegraph

local Parallax = {
	__index = nil,
	hide = nil,
	move = nil,
	set_layer_velocity = nil,
	set_parent = nil,
	show = nil
}

Parallax.__index = setmetatable(Parallax, {
	__call = function(Parallax, layers)
		local self = setmetatable({
			layers = nil,
			node = scenegraph:add_node()
		}, Parallax)
		local f = function(layer)
			local batch, vx, vy = table_unpack(layer)
			return { scenegraph:add_batch(self.node, batch), vx, vy }
		end
		self.layers = F.map(f, layers)
		return self
	end,
	__gc = function(self)
		scenegraph:remove(self.node)
		for i = 1, #self.layers do
			self.layers[i] = nil
		end
		self.layers = nil
	end
})

function Parallax:hide()
	scenegraph:disable(self.node)
end

function Parallax:move(v)
	for i = 1, #self.layers do
		local layer, vx, vy = table_unpack(self.layers[i])
		scenegraph:move(layer, v * vx, v * vy)
	end
end

function Parallax:set_layer_velocity(layer, vx, vy)
	layer = self.layers[layer]
	layer[2] = vx
	layer[3] = vy or 0
end

function Parallax:set_parent(parent)
	scenegraph:set_parent(parent, self.node)
end

function Parallax:show()
	scenegraph:enable(self.node)
end

return Parallax
