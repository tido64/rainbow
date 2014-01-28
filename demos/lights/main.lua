-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
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
	sprite.node = rainbow.scenegraph:add_batch(background)
	g_scene.background = background

	local diffuse = rainbow.shaders.diffuse(true)
	diffuse:set_cutoff(1000)
	local screen = rainbow.platform.screen
	diffuse:set_position(screen.width * 0.5, screen.height * 0.5)
	diffuse:set_radius(1000)
	rainbow.scenegraph:attach_program(sprite.node, diffuse)
	g_scene.lighting = diffuse

	g_scene.delegate = {
		key_down = function() end,
		key_up = function() end,
		touch_began = function() end,
		touch_canceled = function() end,
		touch_ended = function() end,
		touch_moved = function() end,
	}
	rainbow.input.subscribe(g_scene.delegate)
end

function tick()
end

function update()
	g_scene.delegate.touch_moved = function(self, touches)
		for h,t in pairs(touches) do
			g_scene.lighting:set_position(t.x, t.y, 100)
			break
		end
	end
	update = tick
end
