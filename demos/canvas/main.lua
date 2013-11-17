-- Canvas Demo
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

require("Utils")  -- Rainbow (debugging) utilities
require("Input")  -- Input event handler

local assets   = nil  -- Sprite sheet
local batch    = nil  -- Sprite batch with only one sprite
local bgm      = nil  -- Background music
local canvas   = nil  -- Canvas
local check    = 30
local original = nil  -- Backdrop

function init()
	assets = rainbow.texture("canvas.png")

	batch = rainbow.spritebatch(1)
	batch:set_texture(assets)

	local screen = rainbow.platform.screen
	local width = 960
	local height = 640

	original = batch:add(0, 0, width, height)
	original:set_position(screen.width * 0.5, screen.height * 0.5)
	original:set_scale(screen.width / width)

	local blurred = assets:create(1088, 0, width, height)
	local brush   = assets:create(0, 919, 105, 105)

	canvas = rainbow.canvas()
	canvas:set_brush(assets, brush)
	canvas:set_brush_size(64)
	canvas:set_foreground(0x000000ff)
	canvas:set_background(assets, blurred, width, height)

	canvas:clear()

	rainbow.scenegraph:add_batch(batch)
	rainbow.scenegraph:add_drawable(canvas)

	bgm = rainbow.audio.load_stream("kimjongil.ogg");
	rainbow.audio.play(bgm);
end

function update()
	check = check - 1
	if check == 0 then
		print("Current fill percentage: " .. canvas:get_filled())
		check = 30
	end
end
