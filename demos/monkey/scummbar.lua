-- Monkey Demo: SCUMM Bar
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Kvad = require("Kvad")  -- Kvad module lets you create scenes from a table
local scenedef = require("scummbar.kvad")
local screen = rainbow.platform.screen

SceneSCUMMBar = {}
SceneSCUMMBar.__index = SceneSCUMMBar

function SceneSCUMMBar:new()
	local self = setmetatable({
		scene = Kvad.from_table(scenedef),
		edge_margin = 24 * global_scale,
		screen = screen.width,
		split = screen.width
	}, SceneSCUMMBar)
	if not threepwood then
		threepwood = Threepwood(self.scene.resources.monkey)
	end
	return self
end

setmetatable(SceneSCUMMBar, { __call = SceneSCUMMBar.new })

function SceneSCUMMBar:init()
	playSound(self.scene.resources.bgm)
	rainbow.input.subscribe(self)
	threepwood:init(self.scene.node, global_scale * 30, global_scale * 30)
	local objects = self.scene.objects
	objects.chandelier.animations.drink:play()
	objects.bythewindow.animations.drink:play()
	objects.intheback.animations.drink:play()
end

function SceneSCUMMBar:update(dt)
	local threepwood = threepwood
	threepwood:update(dt)

	-- Determine whether we should be panning the screen.
	threepwood.lock = self.screen ~= self.split
	if threepwood.lock then
		local v = 0.6 * dt
		if self.screen < self.split then  -- left to right
			v = v * -1
			self.split = math.max(0, self.split + v)
		else  -- right to left
			self.split = math.min(screen.width, self.split + v)
		end
		rainbow.scenegraph:move(self.scene.node, v, 0)
	else
		local x = threepwood:get_position()
		if x > screen.width - self.edge_margin * 0.5 then
			self.screen = 0
		elseif x < self.edge_margin * 0.5 then
			self.screen = screen.width
		end
	end
end

function SceneSCUMMBar:pointer_began(pointers)
	local walk = nil
	for h,p in pairs(pointers) do
		walk = p
		break
	end

	-- If Guybrush is walking off-screen, pan the scene if possible. To prevent
	-- the scene from panning back and forth, make him walk an extra distance.
	if self.split == screen.width and walk.x > self.split - self.edge_margin then
		walk.x = self.split + self.edge_margin * 1.2
	elseif self.split == 0 and walk.x < self.edge_margin then
		walk.x = self.edge_margin * -1.2
	end
	threepwood:move(walk)
end

function SceneSCUMMBar:destruct() end
function SceneSCUMMBar:key_down() end
function SceneSCUMMBar:key_up() end
function SceneSCUMMBar:pointer_canceled() end
function SceneSCUMMBar:pointer_ended() end
function SceneSCUMMBar:pointer_moved() end
