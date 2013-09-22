--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

local Coroutine = require('Coroutine')

function routine()
	print('coroutine started')
	Coroutine.wait(1000)
	print('1 second elapsed')
	Coroutine.wait(1000)
	print('2 seconds elapsed')
	Coroutine.wait(1000)
	print('3 seconds elapsed')
	Coroutine.wait(1000)
	print('coroutine end')
end

function init()
	Coroutine.start(routine)
end

function update(dt)
	-- Nothing here
end
