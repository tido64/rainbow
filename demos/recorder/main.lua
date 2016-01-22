-- Recorder Demo
--
-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local g_scene = nil

function clamp(v, min, max)
    return math.min(max, math.max(min, v))
end

function init()
    local assets = rainbow.texture("canvas.png")

    local batch = rainbow.spritebatch(2)
    batch:set_texture(assets)

    local screen = rainbow.platform.screen
    local center = { x = screen.width * 0.5, y = screen.height * 0.5 }
    local width = 960
    local height = 640
    local scale = screen.height / height

    local sprite = batch:create_sprite(width, height)
    sprite:set_position(center.x, center.y)
    sprite:set_scale(scale)
    sprite:set_texture(assets:create(0, 0, width, height))

    sprite = batch:create_sprite(width, height)
    sprite:set_position(center.x, center.y)
    sprite:set_scale(scale)
    sprite:set_texture(assets:create(1088, 0, width, height))

    rainbow.scenegraph:add_batch(batch)

    local recorder = rainbow.audio.recorder()
    recorder:record()

    g_scene = {
        batch    = batch,
        blurred  = sprite,
        recorder = recorder,
    }

    print("Note: This is an Android- and iOS-only demo")
end

function update()
    local recorder = g_scene.recorder
    recorder:update()
    print("Avg./Peak/Low-pass: " .. recorder:get_average_power()
                          .. "/" .. recorder:get_peak_power()
                          .. "/" .. recorder:get_low_pass())
    local peak = clamp(recorder:get_peak_power() - recorder:get_low_pass(), -50.0, 0.0)
    g_scene.blurred:set_color(0xff, 0xff, 0xff, math.floor(peak * -5))
end
