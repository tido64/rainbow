--! Transitions Demo
--!
--! Resolution: 960x640
--!
--! Copyright 2012-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require("Kim")
require("Transition")

local batch = nil  -- Sprite batch
local kim   = nil  -- Kim so happy

function init()
	local assets = rainbow.texture("canvas.png")
	batch = rainbow.spritebatch(1)
	batch:set_texture(assets)
	kim = Kim.new(batch:add(448, 108, 100, 100))

	kim.node = rainbow.scenegraph:add_batch(batch)
end

function update(dt)
end
