-- Transitions Demo: Kim
--
-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Math = require("Math")
local Transition = require("Transition")

Kim = {}
Kim.__index = Kim

function Kim.new(sprite)
    local self = {}
    setmetatable(self, Kim)
    self.origin = {0, 0}
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

function Kim:key_down(key, modifiers)
    local distance = 64
    local moved = false
    if key == 1 then  -- Left / A
        self.origin[1] = self.origin[1] - distance
        moved = true
    elseif key == 4 then  -- Right / D
        self.origin[1] = self.origin[1] + distance
        moved = true
    elseif key == 19 then  -- Down / S
        self.origin[2] = self.origin[2] - distance
        moved = true
    elseif key == 23 then  -- Up / W
        self.origin[2] = self.origin[2] + distance
        moved = true
    end
    if moved then
        local screen = rainbow.platform.screen
        rainbow.renderer.set_projection(
            self.origin[1],
            self.origin[2],
            self.origin[1] + screen.width,
            self.origin[2] + screen.height)
    end
end

function Kim:key_up()
end

function Kim:pointer_began(pointers)
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
        for h,p in pairs(pointers) do
            local x, y = self.sprite:get_position()
            self.moving = Transition.move(self.node, p.x - x, p.y - y, 500, Transition.Functions.easeinout_cubic)
            self.alpha = Transition.fadeto(self.sprite, 0xff, 500)
            self.alternate = true
            break
        end
    end
end

-- We don't care about these events.

function Kim:pointer_canceled() end
function Kim:pointer_ended() end
function Kim:pointer_moved() end
