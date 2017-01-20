-- Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local FMOD = FMOD
local sounds = nil

local NUM1 = 49
local NUM2 = 50
local NUM3 = 51

function key_down(_, e)
    local k, _ = e
    if k == NUM1 then
        FMOD.playSound(sounds.drumloop)
    elseif k == NUM2 then
        FMOD.playSound(sounds.jaguar)
    elseif k == NUM3 then
        FMOD.playSound(sounds.swish)
    end
end

function noop() end

function init()
    sounds = {
        drumloop = FMOD.createSound("media/drumloop.wav"),
        jaguar = FMOD.createSound("media/jaguar.wav"),
        swish = FMOD.createSound("media/swish.wav"),
    }
    sounds.drumloop:setLoopCount(0)

    local input_delegate = {
        key_down = key_down,
        key_up = noop,
        pointer_began = noop,
        pointer_canceled = noop,
        pointer_ended = noop,
        pointer_moved = noop,
    }
    rainbow.input.subscribe(input_delegate)

    print("Press \"1\" to play a mono sound (drumloop)")
    print("Press \"2\" to play a mono sound (jaguar)")
    print("Press \"3\" to play a stereo sound (swish)")
end

function update(dt)
    collectgarbage("step")
end
