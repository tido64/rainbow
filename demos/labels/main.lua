-- Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)
--
-- > OpenSans-Light.ttf
-- Copyright (c) 2011 Steve Matteson
--
-- Licensed under the Apache License, Version 2.0 (the “License”);
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--   http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an “AS IS” BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

local Coroutine = require("Coroutine")

local g_label
local screen = rainbow.platform.screen
local y_offset = { 0.55, 0.63, 0.71 }

function change(dt, label)
    local rand = rainbow.random
    local text = {
        "Open Sans\nAaBbCcDdEeFfGgHhIi\nJjKkLlMmNnOoPpQqRr\nSsTtUuVvWwXxYyZz",
        "Grumpy wizards make\ntoxic brew for the\nevil Queen and Jack.",
        "The quick brown fox jumps\nover the lazy dog."
    }
    local num_texts = #text
    local i = 0
    while true do
        local stanza = text[i + 1]
        local _,count = stanza:gsub("\n", "\n")
        label:set_position(screen.width * 0.5, screen.height * y_offset[count])
        label:set_text(stanza)
        i = (i + 1) % num_texts
        Coroutine.wait(5000)
        -- Test that colour is set for future strings
        label:set_color(rand(256), rand(256), rand(256), 255)
        Coroutine.wait(0)
    end
end

function init()
    local rainbow = rainbow
    local label = rainbow.label()
    label:set_alignment('c')
    label:set_font(rainbow.font("OpenSans-Light.ttf", 60))
    rainbow.renderqueue:add(label)
    Coroutine.start(change, label)
    rainbow.seed(0)
    g_label = label
end

function update(dt) end
