-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Coroutine = require("Coroutine")

local MAX_NUM_SPRITES = 256
local NUM_BATCHES = 256

local g_scene = {
	batches = table.create and table.create(NUM_BATCHES) or {},
	routine = nil,
	sprites = table.create and table.create(MAX_NUM_SPRITES * NUM_BATCHES) or {}
}
local g_timer = 0

local function shake()
	local pi = math.pi
	local random = rainbow.random
	local sprites = g_scene.sprites
	while true do
		for i = 1, #sprites do
			sprites[i]:set_rotation(random(pi))
		end
		Coroutine.wait(0)
	end
end

local function spawn()
	local rainbow = rainbow
	local width = rainbow.platform.screen.width
	local height = rainbow.platform.screen.height
	local random = rainbow.random

	local batches = g_scene.batches
	local sprites = g_scene.sprites

	local atlas = rainbow.texture("canvas.png")
	local texture = atlas:create(448, 108, 100, 100)

	for i = 1, NUM_BATCHES do
		local batch = rainbow.spritebatch(256)
		rainbow.scenegraph:add_batch(batch)
		batch:set_texture(atlas)
		batches[i] = batch
	end

	for i = 1, NUM_BATCHES do
		batch = batches[i]
		for j = 1, MAX_NUM_SPRITES do
			local sprite = batch:create_sprite(32, 32)
			sprite:set_position(random(width), random(height))
			sprite:set_texture(texture)
			sprites[(i - 1) * MAX_NUM_SPRITES + j] = sprite
		end
		Coroutine.wait(50)
	end

	print("Sprites count: " .. NUM_BATCHES * MAX_NUM_SPRITES)
end

function init()
	rainbow.seed(0)
	collectgarbage("stop")
	Coroutine.start(spawn)
	Coroutine.start(shake)
end

function update(dt)
	g_timer = g_timer + dt
	if g_timer >= 20000 then
		--error("Done")
	end
end
