--! Monkey Demo: Intro
--!
--! Copyright 2011-13 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local scenedef = require("intro.prose")

SceneIntro = {}
SceneIntro.__index = SceneIntro

function SceneIntro:new()
	--[[
	This is just a short-hand version of writing:

		local self = {}
		setmetatable(self, SceneIntro)
		self.scene = etc.
	--]]
	local self = setmetatable({
		scene = rainbow.prose.from_table(scenedef),
		timer = 0
	}, SceneIntro)
	rainbow.scenegraph:disable(self.scene.objects.logo_batch.node)
	return self
end

--[[
This here lets you create a SceneIntro object without calling .new:

	local scene = SceneIntro()
--]]
setmetatable(SceneIntro, { __call = SceneIntro.new })

function SceneIntro:destruct()
	--[[
		We've already marked self.scene for collection. There's nothing else to
		do here.
	--]]
end

function SceneIntro:init()
	rainbow.audio.play(self.scene.resources.bgm)
	self.timer = rainbow.timer(self, 22000, 1)
end

function SceneIntro:tick()
	rainbow.input.subscribe(self)
	rainbow.scenegraph:enable(self.scene.objects.logo_batch.node)
end

function SceneIntro:touch_began()
	rainbow.input.unsubscribe(self);
	self.scene = nil;
end

function SceneIntro:update(dt)
	if not self.scene then
		return "scummbar";
	end
end

function SceneIntro:key_down() end
function SceneIntro:key_up() end
function SceneIntro:touch_canceled() end
function SceneIntro:touch_ended() end
function SceneIntro:touch_moved() end
