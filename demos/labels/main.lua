-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Coroutine = require("Coroutine")

function change(dt, label)
	local text = {
		"The path of the righteous man\nis beset on all sides\nby the iniquities of the selfish\nand the tyranny of evil men.",
		"Blessed is he who,\nin the name of charity and good will,\nshepherds the weak through the valley of darkness,\nfor he is truly his brother's keeper\nand the finder of lost children.",
		"And I will strike down upon thee\nwith great vengeance and furious anger\nthose who would attempt to poison\nand destroy My brothers.",
		"And you will know\nMy name is the Lord\nwhen I lay My vengeance\nupon thee."
	}
	local num_texts = #text
	local i = 0
	while true do
		local stanza = text[i + 1]
		--local _,count = stanza:gsub("\n", "\n")
		label:set_text(stanza)
		i = (i + 1) % num_texts
		Coroutine.wait(5000)
	end
end

function init()
	local label = rainbow.label()
	label:set_alignment('c')
	label:set_font(rainbow.font("Inconsolata.otf", 32))
	local screen = rainbow.platform.screen
	label:set_position(screen.width * 0.5, screen.height * 0.6)
	node = rainbow.scenegraph:add_label(label)
	Coroutine.start(change, label)
end

function update(dt) end
