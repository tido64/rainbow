--! Monkey Demo
--!
--! Resolution: 960x640
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require("Utils");  -- Rainbow (debugging) utilities
require("Input");  -- Input event handler

global_scale = 3;  -- Global scaling factor
threepwood = nil;  -- Guybrush Threepwood

local assets = nil;  -- Sprite sheet
local scene = nil;   -- Current scene
local scenes = {};   -- Table o'scenes

require("threepwood");  -- I'm Guybrush Threepwood. A mighty pirate.
require("intro");       -- Intro scene
require("scummbar");    -- SCUMM Bar scene

function init()
	-- Declare all scenes
	scenes["opening"] = SceneIntro;
	scenes["scummbar"] = SceneSCUMMBar;

	-- Load common assets
	assets = rainbow.texture("monkey.png");

	-- Load Guybrush Threepwood. He should be reusable in all scenes.
	threepwood = Threepwood.new(assets);

	-- Load opening scene
	scene = scenes["opening"].new(assets);
	scene:init();
end

function update(dt)
	local next_scene = scene:update(dt);
	if next_scene then
		-- Delete current scene
		scene:destruct();

		-- Load and initialize next scene
		scene = scenes[next_scene].new(assets);
		scene:init();
	end
end
