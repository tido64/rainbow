--! Rainbow accelerometer utilities

--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

rainbow.utils.accelerometer = {};

_rainbow_utils_accelerometer_enabled = false;
_rainbow_utils_accelerometer_init = false;

function rainbow.utils.accelerometer.disable()
	_rainbow_utils_accelerometer_enabled = false;
end

function rainbow.utils.accelerometer.enable()
	if _rainbow_utils_accelerometer_enabled then
		return
	end
	if not _rainbow_utils_accelerometer_init then
		_rainbow_utils_accelerometer_batch = rainbow.spritebatch(1);
		_rainbow_utils_accelerometer_batch_red = rainbow.spritebatch(1);
		_rainbow_utils_accelerometer_batch_blue = rainbow.spritebatch(1);

		_rainbow_utils_accelerometer_batch:set_texture("axis.png");
		_rainbow_utils_accelerometer_batch_red:set_texture("red.png");
		_rainbow_utils_accelerometer_batch_blue:set_texture("blue.png");

		local x = rainbow.platform.screen.width * 0.5;
		local y = rainbow.platform.screen.height * 0.5;

		_rainbow_utils_accelerometer_axis = _rainbow_utils_accelerometer_batch:add(0, 0, 612, 613);
		_rainbow_utils_accelerometer_axis:set_position(x, y);

		_rainbow_utils_accelerometer_red = _rainbow_utils_accelerometer_batch_red:add(0, 0, 58, 57);
		_rainbow_utils_accelerometer_red:set_position(x, y);

		_rainbow_utils_accelerometer_blue = _rainbow_utils_accelerometer_batch_blue:add(0, 0, 58, 57);
		_rainbow_utils_accelerometer_blue:set_position(x, y);

		_rainbow_utils_accelerometer_init = true;
	end
	_rainbow_utils_accelerometer_enabled = true;
end

function rainbow.utils.accelerometer.draw()
	if not _rainbow_utils_accelerometer_enabled then
		return
	end
	_rainbow_utils_accelerometer_batch:draw();
	_rainbow_utils_accelerometer_batch_red:draw();
	_rainbow_utils_accelerometer_batch_blue:draw();
end

function rainbow.utils.accelerometer.update()
	if not _rainbow_utils_accelerometer_enabled or not rainbow.input.acceleration then
		return
	end
	local accel = rainbow.input.acceleration;
	local screen = rainbow.platform.screen;
	local x = screen.width * 0.5 * accel.x;
	local y = screen.height * 0.5 * accel.y;
	--proto_blue:set_position(318 + x, 482);
	--proto_red:set_position(318, 482 + y);
	_rainbow_utils_accelerometer_batch:update();
	_rainbow_utils_accelerometer_batch_blue:update();
	_rainbow_utils_accelerometer_batch_red:update();
end
