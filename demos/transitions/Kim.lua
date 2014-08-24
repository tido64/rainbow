-- Transitions Demo: Kim
--
-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Math = require("Math")
local Transition = require("Transition")

Kim = {}
Kim.__index = Kim

function Kim.new(sprite)
	local self = {}
	setmetatable(self, Kim)
	self.sprite = sprite

	-- Used to alternate between move and rotate.
	self.alternate = false

	-- Cache rainbow namespace.
	local rainbow = rainbow

	-- Move sprite to the centre of the screen.
	local screen = rainbow.platform.screen
	self.moving = Transition.move(self.sprite, screen.width * 0.5, screen.height * 0.5, 400)

	-- Make sprite transparent while rotating, and opaque while moving.
	self.sprite:set_color(0xff, 0xff, 0xff, 0x00)
	self.alpha = Transition.fadeto(self.sprite, 0xff, 400)

	-- Subscribe this class to input events.
	rainbow.input.subscribe(self)

	return self
end

function Kim:touch_began(touches)
	-- Cancel the previous transitions.
	self.alpha:cancel()
	self.moving:cancel()

	if self.alternate then
		-- Rotate 360 degrees in 1 second. Linear effect.
		local angle = self.sprite:get_angle() + Math.radians(360)
		self.moving = Transition.rotate(self.sprite, angle, 1000)
		self.alpha = Transition.fadeto(self.sprite, 0x40, 1000)
		self.alternate = false
	else
		-- Move to point in 0.5 seconds. Squared ease-in effect.
		for h,t in pairs(touches) do
			local x, y = self.sprite:get_position()
			self.moving = Transition.move(self.node, t.x - x, t.y - y, 500, Transition.Functions.easeinout_cubic)
			self.alpha = Transition.fadeto(self.sprite, 0xff, 500)
			self.alternate = true
			break
		end
	end
end

-- We don't care about these events.

function Kim:key_down() end
function Kim:key_up() end
function Kim:touch_canceled() end
function Kim:touch_ended() end
function Kim:touch_moved() end
