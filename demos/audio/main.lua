-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local Coroutine = require('Coroutine')

function test()
	local rainbow_audio = rainbow.audio
	local file = "maja_id.ogg"

	print("Test: Load music")
	local music = rainbow_audio.create_sound(file, 1)
	Coroutine.wait(5000)

	print("Test: Play music")
	local ch = rainbow_audio.play(music)
	Coroutine.wait(5000)

	print("Test: Pause music")
	rainbow_audio.pause(ch)
	Coroutine.wait(3000)

	print("Test: Resume music")
	rainbow_audio.pause(ch)
	Coroutine.wait(5000)

	print("Test: Stop music")
	rainbow_audio.stop(ch)
	Coroutine.wait(500)

	print("Test: Load sfx")
	local sound = { rainbow_audio.create_sound(file) }
	Coroutine.wait(3000)

	print("Test: Play sfx")
	ch = rainbow_audio.play(sound[1])
	Coroutine.wait(5000)

	print("Test: Pause sfx")
	rainbow_audio.pause(ch)
	Coroutine.wait(3000)

	print("Test: Resume sfx")
	rainbow_audio.pause(ch)
	Coroutine.wait(5000)

	print("Test: Play music (different channel)")
	ch = rainbow_audio.play(music)
	Coroutine.wait(5000)

	print("Test: Delete music (while it's playing)")
	rainbow_audio.delete_sound(music)
	Coroutine.wait(5000)

	print("Test: Load and play sfx (overflow)")
	for i = 2, 40 do  -- ConFuoco is currently hardwired to 32 channels
		sound[i] = rainbow_audio.create_sound(file)
		rainbow_audio.play(sound[i])
	end
	Coroutine.wait(5000)

	print("Test: Clear")
	rainbow_audio.clear()
	sound = nil
	Coroutine.wait(3000)

	print("Test: Loop test (once)")
	music = rainbow_audio.create_sound(file, 1, 1)
	ch = rainbow_audio.play(music)
	Coroutine.wait(1000 * 60 * 10)
end

function init()
	Coroutine.start(test)
end

function update(dt) end
