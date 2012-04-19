--! Parallax
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local frame_ms = 60 / 1000
local max_dt = 50

Parallax = {}
Parallax.__index = Parallax

function Parallax.new(parent, layers)
	if not parent or not layers then
		return
	end

	local self = {}
	setmetatable(self, Parallax)

	self.batches = {}
	self.count = 0
	self.last_texture = nil

	-- Create parallax root node
	self.scene = rainbow.scenegraph
	self.root = self.scene:add_node(parent)
	self.parent = parent

	-- Create all the layer nodes
	self.layer_count = layers
	self.layers = {}
	self.velocities = {}
	for i = layers, 1, -1 do
		self.layers[i] = self.scene:add_node(self.root)
		self.velocities[i] = { 0, 0 }
	end

	return self
end

function Parallax:add(layer, texture, x, y, width, height, hint)
	if texture and texture ~= self.last_texture then
		local rainbow = rainbow
		local i = self.count + 1

		self.batches[i] = rainbow.spritebatch(hint or 1)
		self.batches[i]:set_texture(texture)
		self.scene:add_batch(self.layers[layer], self.batches[i])

		self.last_texture = texture
		self.count = i
	end
	return self.batches[self.count]:add(x, y, width, height)
end

function Parallax:destruct()
	self.scene:remove(self.parent, self.root)
	self.parent = nil
	self.root = nil
	self.layers = nil
	for i = 1, self.count do
		self.batches[i] = nil
	end
	self.last_texture = nil
end

function Parallax:hide()
	self.scene:disable(self.root)
end

function Parallax:set_layer_velocity(layer, x, y)
	self.velocities[layer] = { x, y }
end

function Parallax:show()
	self.scene:enable(self.root)
end

function Parallax:update(dt)
	local scale = math.min(max_dt, dt * frame_ms)
	for i = 1, self.layer_count do
		local v = self.velocities[i]
		self.scene:move(self.layers[i], v[1] * scale, v[2] * scale)
	end
end
