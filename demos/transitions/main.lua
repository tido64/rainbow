-- Transitions Demo
--
-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

require("Kim")

local batch = nil  -- Sprite batch
local kim   = nil  -- Kim so happy

function init()
    local assets = rainbow.texture("canvas.png")
    batch = rainbow.spritebatch(1)
    batch:set_texture(assets)
    kim = Kim.new(batch:add(448, 108, 100, 100))

    kim.unit = rainbow.renderqueue:add(batch)
end

function update(dt)
end
