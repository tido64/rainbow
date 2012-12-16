local ch = nil
local count = 0
local file = "maja_id.ogg"
local next_stage = 5000
local music = nil
local sound = nil
local stage = 1

function init()
	print("Test: Load music")
	music = rainbow.audio.create_sound(file, 1)
	print("Test: Play music")
	ch = rainbow.audio.play(music)
end

function test()
	if stage == 1 then
		print("Test: Pause music")
		rainbow.audio.pause(ch)
	elseif stage == 2 then
		print("Test: Resume music")
		rainbow.audio.pause(ch)
	elseif stage == 3 then
		print("Test: Stop music")
		rainbow.audio.stop(ch)
		next_stage = 500
	elseif stage == 4 then
		print("Test: Load sfx")
		sound = rainbow.audio.create_sound(file)
		print("Test: Play sfx")
		rainbow.audio.play(sound)
		next_stage = 5000
	elseif stage == 5 then
		print("Test: Play music (different channel)")
		ch = rainbow.audio.play(music)
	elseif stage == 6 then
		print("Test: Delete music (while it's playing)")
		rainbow.audio.delete_sound(music)
	elseif stage == 7 then
		print("Test: Clear")
		rainbow.audio.clear()
	elseif stage == 8 then
		print("Test: Loop test (once)")
		music = rainbow.audio.create_sound(file, 1, 1)
		ch = rainbow.audio.play(music)
		next_stage = 2147483646
	end
	stage = stage + 1
end

function update(dt)
	count = count + dt
	if count > next_stage then
		test()
		count = 0
	end
end
