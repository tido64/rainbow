-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

g_scene = g_scene or {}

function init()
    local assets = rainbow.texture("normal_map_test.png")
    local background = rainbow.spritebatch(1)
    background:set_normal(assets)
    background:set_texture(assets)
    local screen = rainbow.platform.screen
--[[
    local sprite = background:create_sprite(512, 440)
    sprite:set_position(screen.width * 0.5, screen.height * 0.5)
    sprite:set_normal(assets:create(1, 443, 512, 440))
    sprite:set_texture(assets:create(1, 1, 512, 440))
--]]
    local sprite = background:create_sprite(450, 338)
    sprite:set_position(screen.width * 0.5, screen.height * 0.5)
    sprite:set_normal(assets:create(515, 341, 450, 338))
    sprite:set_texture(assets:create(515, 1, 450, 338))
    local sprite_node = rainbow.scenegraph:add_batch(background)
    g_scene.background = background

    local diffuse = rainbow.shaders.diffuse(true)
    diffuse:set_cutoff(1000)
    local screen = rainbow.platform.screen
    diffuse:set_position(screen.width * 0.5, screen.height * 0.5)
    diffuse:set_radius(1000)
    rainbow.scenegraph:attach_program(sprite_node, diffuse)
    g_scene.lighting = diffuse

    g_scene.delegate = {
        key_down = function() end,
        key_up = function() end,
        pointer_began = function() end,
        pointer_canceled = function() end,
        pointer_ended = function() end,
        pointer_moved = function() end,
    }
    rainbow.input.subscribe(g_scene.delegate)
end

function tick()
end

function update()
    g_scene.delegate.pointer_moved = function(self, pointers)
        for h,p in pairs(pointers) do
            g_scene.lighting:set_position(p.x, p.y, 100)
            break
        end
    end
    update = tick
end
