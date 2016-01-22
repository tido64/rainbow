-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local g_body_def
local g_box_shape
local g_circle_shape
local g_mouse_down
local g_ptm
local g_world

local create_body_at
local create_circle_at

local function create_box_at(pointers)
    for _,p in pairs(pointers) do
        g_body_def.position = {x = p.x / g_ptm, y = p.y / g_ptm}
        local box = g_world:CreateBody(g_body_def)
        box:CreateFixture(g_box_shape, 50)
        box:ResetMassData()
    end
    create_body_at = create_circle_at
end

create_circle_at = function(pointers)
    for _,p in pairs(pointers) do
        g_body_def.position = {x = p.x / g_ptm, y = p.y / g_ptm}
        local box = g_world:CreateBody(g_body_def)
        box:CreateFixture(g_circle_shape, 50)
        box:ResetMassData()
    end
    create_body_at = create_box_at
end

local function on_mouse_up()
    g_mouse_down = false
end

local input_handler = {
    key_down = function() end,
    key_up = function() end,
    pointer_began = function(_, pointers)
        g_mouse_down = true
        create_body_at(pointers)
    end,
    pointer_ended = on_mouse_up,
    pointer_canceled = on_mouse_up,
    pointer_moved = function(_, pointers)
        if g_mouse_down then
            create_body_at(pointers)
        end
    end,
}

function init()
    local rainbow = rainbow
    local screen = rainbow.platform.screen

    local scale = screen.width / 1280

    g_world = b2.World()
    g_ptm = g_world:GetPTM() * scale  -- Default: 32 px/m
    g_world:SetPTM(g_ptm)
    g_world:SetDebugDraw(true)

    g_body_def = b2.BodyDef()
    local ground = g_world:CreateBody(g_body_def)
    local shape = b2.EdgeShape()
    shape:Set(40.0 * scale / g_ptm,
              10.0 * scale / g_ptm,
              (screen.width - 40.0 * scale) / g_ptm,
              10.0 * scale / g_ptm)
    ground:CreateFixture(shape, 0.0)

    g_body_def.type = b2.dynamicBody

    local a = 0.5
    g_box_shape = b2.PolygonShape()
    g_box_shape:SetAsBox(a, a)

    g_circle_shape = b2.CircleShape()
    g_circle_shape:SetRadius(a)

    create_body_at = create_box_at
    rainbow.input.subscribe(input_handler)
end

function update(dt)
    g_world:Step(dt)
end
