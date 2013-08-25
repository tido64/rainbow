--! Copyright 2013 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

require('Coroutine')

function routine()
	print('coroutine started')
	rainbow.coroutine.wait(1000)
	print('1 second elapsed')
	rainbow.coroutine.wait(1000)
	print('2 seconds elapsed')
	rainbow.coroutine.wait(1000)
	print('3 seconds elapsed')
	rainbow.coroutine.wait(1000)
	print('coroutine end')
end

function init()
	rainbow.coroutine.start(routine)
end

function update(dt)
	-- Nothing here
end
