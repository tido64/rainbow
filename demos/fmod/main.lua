-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local FMOD = FMOD
local events = nil

local NUM1 = 49
local NUM2 = 50
local NUM3 = 51
local NUM4 = 52

function key_down(_, e)
	local k, _ = e
	if k == NUM1 then
		local instance = FMOD.createInstance("event:/Explosions/Single Explosion")
		instance:start()
		-- 'instance' is released when collected
	elseif k == NUM2 then
		events.ambience:start()
	elseif k == NUM3 then
		events.ambience:stop()
	elseif k == NUM4 then
		events.cancel:start()
	end
end

function noop() end

function init()
	FMOD.loadBank("media/Master Bank.bank")
	FMOD.loadBank("media/Master Bank.strings.bank")
	FMOD.loadBank("media/Surround_Ambience.bank")
	FMOD.loadBank("media/UI_Menu.bank")
	FMOD.loadBank("media/Weapons.bank")
	events = {
		ambience = FMOD.createInstance("event:/Ambience/Country"),
		cancel = FMOD.createInstance("event:/UI/Cancel")
	}

	local input_delegate = {
		key_down = key_down,
		key_up = noop,
		touch_began = noop,
		touch_canceled = noop,
		touch_ended = noop,
		touch_moved = noop,
	}
	rainbow.input.subscribe(input_delegate)

	print("Press \"1\" to fire and forget the explosion")
	print("Press \"2\" to start the looping ambience")
	print("Press \"3\" to stop the looping ambience")
	print("Press \"4\" to start/restart the cancel sound")
end

function update(dt)
	collectgarbage("step")
end
