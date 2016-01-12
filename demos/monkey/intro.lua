-- Monkey Demo: Intro
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Kvad = require("Kvad")  -- Kvad module lets you create scenes from a table
local Timer = require("Timer")
local scenedef = require("intro.kvad")

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
		scene = Kvad.from_table(scenedef),
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
	playSound(self.scene.resources.bgm)
	local callback = function()
		self:tick()
	end
	self.timer = Timer(callback, 22000, 1)
end

function SceneIntro:tick()
	rainbow.input.subscribe(self)
	rainbow.scenegraph:enable(self.scene.objects.logo_batch.node)
end

function SceneIntro:pointer_began()
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
function SceneIntro:pointer_canceled() end
function SceneIntro:pointer_ended() end
function SceneIntro:pointer_moved() end
