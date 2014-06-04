-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local screen = rainbow.platform.screen
local cx, cy = screen.width * 0.5, screen.height * 0.5
return {
	version = 1.1,
	resources = {
		bgm = "scummbar.ogg",
		monkey = {
			"monkey.png",
			{ 0, 324, 640, 144 },  -- 0: backdrop
			{ 1, 469, 64, 48 },    -- 1: pirate on the chandelier (1/6)
			{ 65, 469, 64, 48 },   -- 2: pirate on the chandelier (2/6)
			{ 129, 469, 64, 48 },  -- 3: pirate on the chandelier (3/6)
			{ 193, 469, 64, 48 },  -- 4: pirate on the chandelier (4/6)
			{ 257, 469, 64, 48 },  -- 5: pirate on the chandelier (5/6)
			{ 321, 469, 64, 48 },  -- 6: pirate on the chandelier (6/6)
			{ 195, 518, 48, 48 },  -- 7: pirates by the window (1/3)
			{ 243, 518, 48, 48 },  -- 8: pirates by the window (2/3)
			{ 291, 518, 48, 48 },  -- 9: pirates by the window (3/3)
			{ 340, 518, 32, 40 },  -- 10: pirate chillin' in the back (1,6/6)
			{ 372, 518, 32, 40 },  -- 11: pirate chillin' in the back (2,5/6)
			{ 404, 518, 32, 40 }   -- 12: pirate chillin' in the back (3,4/6)
		}
	},
	nodes = {
		{  -- Mostly backdrop and NPCs.
			name = "bar",
			texture = "monkey",
			sprites = {
				{  -- Backdrop
					name = "bg",
					size = { 640, 144 },
					position = { screen.width, cy },
					scale = global_scale,
					texture = 0
				},
				{  -- Pirate hanging on the chandelier
					name = "chandelier",
					size = { 64, 48 },
					animations = { drink = { 1, 2, 3, 4, 5, 6, fps = 6 } },
					position = {
						screen.width - global_scale * 136,
						cy + global_scale * 24
					},
					scale = global_scale,
					texture = 1
				},
				{  -- Pirates sitting by the window
					name = "bythewindow",
					size = { 48, 48 },
					animations = { drink = { 7, 8, 9, fps = 2 } },
					position = { global_scale * 144, cy + global_scale * -32 },
					scale = global_scale,
					texture = 7
				},
				{  -- Pirate chillin' in the back
					name = "intheback",
					size = { 32, 40 },
					animations = {
						drink = { 10, 11, 12, 12, 11, 10, fps = 3, delay = 6 }
					},
					position = {
						screen.width - global_scale * 88,
						cy + global_scale * -4
					},
					scale = global_scale,
					texture = 10
				}
			}
		}
	}
}
