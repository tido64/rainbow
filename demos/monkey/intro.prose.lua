-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local screen = rainbow.platform.screen
local cx, cy = screen.width * 0.5, screen.height * 0.5
return {
	version = 1.1,
	resources = {
		bgm = "opening.ogg",  -- auto removed :)
		monkey = {
			"monkey.png",
			{ 0, 0, 320, 200 },    -- 0: island backdrop
			{ 16, 202, 216, 120 }  -- 1: logo
		}
	},
	nodes = {
		{
			name = "island_batch",
			texture = "monkey",
			sprites = {
				{
					name = "island",
					size = { 320, 200 },
					position = { cx, cy },
					scale = global_scale,
					texture = 0
				}
			}
		},
		{
			name = "logo_batch",
			texture = "monkey",
			sprites = {
				{
					name = "logo",
					size = { 216, 120 },
					position = { cx - global_scale * 4, cy + global_scale * 16 },
					scale = global_scale,
					texture = 1
				}
			}
		}
	}
}
