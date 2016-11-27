-- Monkey Demo: Guybrush Threepwood
--
-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local THREEPWOOD = "Threepwood"
local THREEPWOOD_WALKING = "Threepwood walking"

Threepwood = {}
Threepwood.__index = Threepwood

function Threepwood:new(assets)
    local self = setmetatable({
        animating = false,
        batch = rainbow.spritebatch(1),
        direction = false,
        face = 1,
        lock = false,
        node = false,
        sprite = false,
        standingl = assets:create(504, 875, 104, 149),
        standingr = assets:create(400, 875, 104, 149),
        walk = false,
        walking = false,
        walkingl = {},
        walkingr = {},
        x = 0,
        y = 0
    }, Threepwood)

    self.batch:set_texture(assets)
    self.sprite = self.batch:create_sprite(104, 149)
    self.sprite:set_texture(self.standingr)
    self.direction = self.standingr

    -- Load Guybrush walking, facing left or right
    local x = 400
    for i = 1,6 do
        self.walkingl[i] = assets:create(x, 724, 104, 149)
        self.walkingr[i] = assets:create(x, 575, 104, 149)
        x = x + 104
    end
    self.walkingl = rainbow.animation(self.sprite, self.walkingl, 7)
    self.walkingr = rainbow.animation(self.sprite, self.walkingr, 7)

    return self
end

setmetatable(Threepwood, { __call = Threepwood.new })

function Threepwood:destruct()
    rainbow.renderqueue:erase(THREEPWOOD)
    self.batch = nil
    self.walkingl = nil
    self.walkingr = nil
end

function Threepwood:init(x, y)
    self.x = x
    self.y = y
    self.sprite:set_position(self.x, self.y)
    rainbow.renderqueue:add(self.batch, THREEPWOOD)
end

function Threepwood:get_position()
    return self.x, self.y
end

function Threepwood:move(pos)
    if self.lock then
        return
    end

    -- Determine the direction Guybrush is facing and start the walking animation.
    if self.animating then
        rainbow.renderqueue:erase(THREEPWOOD_WALKING)
    end
    self.walk = pos
    if self.x < self.walk.x then
        self.direction = self.standingr
        self.face = 1
        self.walking = self.walkingr
    else
        self.direction = self.standingl
        self.face = -1
        self.walking = self.walkingl
    end
    self.animating = true
    rainbow.renderqueue:add(self.walking, THREEPWOOD_WALKING)
    self.walking:start()
end

function Threepwood:stop()
    rainbow.renderqueue:erase(THREEPWOOD_WALKING)
    self.animating = false
    self.sprite:set_texture(self.direction)
    self.walk = false
end

function Threepwood:update(dt)
    if self.walk then
        -- While Guybrush is walking, check for when he reaches his destination
        -- and stop the animation.
        local dx = global_scale * self.face
        self.x = self.x + dx
        if self.face > 0 and self.x >= self.walk.x or self.face < 0 and self.x <= self.walk.x then
            dx = dx - (self.x - self.walk.x)
            self.x = self.walk.x
            self.walk = false
            self:stop()
        end
        self.sprite:move(dx, 0)
    elseif self.lock then
        -- While Guybrush is locked, get his screen position.
        self.x, self.y = self.sprite:get_position()
    end
end
