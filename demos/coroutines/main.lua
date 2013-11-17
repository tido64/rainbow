-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
