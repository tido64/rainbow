--! Monkey Demo
--!
--! Resolution: 960x640
--!
--! Copyright 2011-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

-- Declare globals used throughout
global_scale = 3  -- Global scaling factor
threepwood = nil  -- Guybrush Threepwood should always be loaded

-- Import scenes
require("intro")     -- Intro scene
require("scummbar")  -- SCUMM Bar

-- Import protagonist
require("threepwood")

local scene = nil   -- Current scene
local scenes = {}   -- Table o'scenes

function init()
	-- Turn off texture filtering.
	rainbow.renderer.set_filter(gl.NEAREST)

	-- Declare all scenes.
	scenes["opening"] = SceneIntro
	scenes["scummbar"] = SceneSCUMMBar

	-- Load opening scene.
	scene = scenes["opening"]()
	scene:init()
end

function update(dt)
	local next_scene = scene:update(dt)
	if next_scene then
		-- Destroy and collect the current scene.
		scene:destruct()
		collectgarbage("collect")

		-- Load and initialize next scene.
		scene = scenes[next_scene]()
		scene:init()
	end
end
