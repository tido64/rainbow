-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Coroutine = require("Coroutine")

local MAX_NUM_SPRITES = 256
local NUM_BATCHES = 256
local SAMPLE_SIZE = 60

local g_frame_idx = 1
local g_frame_times = table.create(SAMPLE_SIZE)
local g_scene = {
    batches = table.create and table.create(NUM_BATCHES) or {},
    routine = nil,
    sprites = table.create and table.create(MAX_NUM_SPRITES * NUM_BATCHES) or {}
}

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

    local current = 1
    while true do
        local frame = g_frame_times[1]
        for i = 2, SAMPLE_SIZE do
            frame = frame + g_frame_times[i]
        end
        if frame / SAMPLE_SIZE < 18 then
            local batch = rainbow.spritebatch(MAX_NUM_SPRITES)
            rainbow.renderqueue:add(batch)
            batch:set_texture(atlas)
            local i = (current - 1) * MAX_NUM_SPRITES
            for j = 1, MAX_NUM_SPRITES do
                local sprite = batch:create_sprite(32, 32)
                sprite:set_position(random(width), random(height))
                sprite:set_texture(texture)
                sprites[i + j] = sprite
            end
            print("Sprites count: " .. current * MAX_NUM_SPRITES)
            batches[current] = batch
            current = current + 1
        end
        Coroutine.wait(50)
    end
end

function init()
    rainbow.seed(0)
    for i = 1, SAMPLE_SIZE do
        g_frame_times[i] = 0
    end
    collectgarbage("stop")
    Coroutine.start(spawn)
    Coroutine.start(shake)
end

function update(dt)
    g_frame_times[g_frame_idx + 1] = dt
    g_frame_idx = (g_frame_idx + 1) % SAMPLE_SIZE
end
