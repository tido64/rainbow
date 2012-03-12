--! Blow Demo
--!
--! Resolution: 960x640
--!
--! Copyright 2012 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require("Utils")  -- Rainbow (debugging) utilities

assets   = nil  -- Sprite sheet
batch    = nil  -- Sprite batch
blurred  = nil  -- Blurred backdrop
original = nil  -- Original, unaltered backdrop
recorder = nil  -- Audio recorder

function init()
	assets = rainbow.texture("canvas.png")

	batch = rainbow.spritebatch(2)
	batch:set_texture(assets)

	local screen = rainbow.platform.screen
	local width = 960
	local height = 640

	original = batch:add(0, 0, width, height)
	original:set_position(screen.width * 0.5, screen.height * 0.5)
	original:set_scale(screen.width / width)

	blurred = batch:add(1088, 0, width, height)
	blurred:set_position(screen.width * 0.5, screen.height * 0.5)
	blurred:set_scale(screen.width / width)

	rainbow.scenegraph:add_batch(batch)

	recorder = rainbow.audio.recorder()
	recorder:record()

	print("Warning: This is an iOS-only demo (for now)")
end

function update()
	local rec = recorder
	rec:update()
	--printf("Avg/Peak/Low-pass: %f/%f/%f\n", rec:get_average_power(), rec:get_peak_power(), rec:get_low_pass())
	local peak = math.min(0.0, math.max(-50.0, rec:get_peak_power() - rec:get_low_pass()))
	local c = math.floor(peak * -5)
	blurred:set_color(0xff, 0xff, 0xff, c)
end
