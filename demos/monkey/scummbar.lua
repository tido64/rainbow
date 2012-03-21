--! Monkey Demo: SCUMM Bar
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

SceneSCUMMBar = {};
SceneSCUMMBar.__index = SceneSCUMMBar;

function SceneSCUMMBar.new(assets)
	local s = {};
	setmetatable(s, SceneSCUMMBar);

	-- Create a sprite batch with 8 sprites
	-- rainbow.spritebatch([hint])
	s.spritebatch = rainbow.spritebatch(8);
	s.spritebatch:set_texture(assets);

	-- Load the backdrop
	s.backdrop = s.spritebatch:add(0, 324, 640, 144);

	-- Load the pirate hanging from the chandelier (6 frames, 6 fps)
	-- Note: This is still stupid because you have to make the first frame twice.
	s.chandelier = s.spritebatch:add(1, 469, 64, 48);
	s.chandelier.animation = {};
	local x = 1;
	for i = 1,6 do
		s.chandelier.animation[i] = assets:create(x, 469, 64, 48);
		x = x + 64;
	end
	-- rainbow.animation(sprite, table o'frames, fps, loop delay = 0);
	s.chandelier.animation = rainbow.animation(s.chandelier, s.chandelier.animation, 6);

	-- Load the two pirates sitting by the window
	s.bythewindow = s.spritebatch:add(195, 518, 48, 48);
	s.bythewindow.animation = {};
	x = 195;
	for i = 1,3 do
		s.bythewindow.animation[i] = assets:create(x, 518, 48, 48);
		x = x + 48;
	end
	s.bythewindow.animation = rainbow.animation(s.bythewindow, s.bythewindow.animation, 2);

	-- Load the pirate chillin' in the back
	s.intheback = s.spritebatch:add(340, 518, 32, 40);
	s.intheback.animation = {};
	x = 340;
	for i = 1,3 do
		s.intheback.animation[i] = assets:create(x, 518, 32, 40);
		x = x + 32;
	end
	-- We reuse the first three frames but in reverse order
	for i = 4,6 do
		s.intheback.animation[i] = s.intheback.animation[7 - i];
	end
	s.intheback.animation = rainbow.animation(s.intheback, s.intheback.animation, 3, 6);

	-- Determines whether we're behind the curtain
	s.position = rainbow.platform.screen.width;
	s.scene = s.position;
	s.margin = 64;

	s.bgm = rainbow.audio.load_stream("scummbar.ogg");
	s.node = nil;

	return s;
end

function SceneSCUMMBar:destruct()
	rainbow.audio.clear();

	rainbow.scenegraph:remove(self.node);
	self.node = nil;

	self.backdrop = nil;
	self.bythewindow = nil;
	self.chandelier = nil;
	self.intheback = nil;
	self.spritebatch = nil;
end

function SceneSCUMMBar:key_down()
end

function SceneSCUMMBar:key_up()
end

function SceneSCUMMBar:init()
	if self.node then
		return
	end

	-- Start SCUMM Bar theme
	rainbow.audio.play(self.bgm);

	-- Local convenience variables
	local screen = rainbow.platform.screen;
	local centre_y = screen.height * 0.5;

	-- Scale and position the backdrop
	self.backdrop:set_scale(global_scale);
	self.backdrop:set_position(screen.width, centre_y);

	-- Scale and position the pirate hanging from the chandelier
	self.chandelier:set_scale(global_scale);
	self.chandelier:set_position(screen.width - global_scale * 136,
	                             centre_y + global_scale * 24);

	-- Scale and position the two pirates sitting by the window
	self.bythewindow:set_scale(global_scale);
	self.bythewindow:set_position(global_scale * 144, centre_y + global_scale * -32);

	-- Scale and position the pirate chillin' in the back
	self.intheback:set_scale(global_scale);
	self.intheback:set_position(screen.width - global_scale * 88,
	                            centre_y + global_scale * -4);

	-- Add this scene to the scene graph
	self.node = rainbow.scenegraph:add_batch(self.spritebatch);

	-- Add animations to this scene node
	rainbow.scenegraph:add_animation(self.node, self.chandelier.animation);
	rainbow.scenegraph:add_animation(self.node, self.bythewindow.animation);
	rainbow.scenegraph:add_animation(self.node, self.intheback.animation);

	threepwood:init(self.node, self.spritebatch, global_scale * 30, global_scale * 30);

	-- Enable input
	rainbow.input.subscribe(self);
end

function SceneSCUMMBar:touch_began(touches)
	local walk = nil;
	for h,t in pairs(touches) do
		walk = t;
		break;
	end

	-- If Guybrush is walking off-screen, pan the scene if possible. To prevent
	-- the scene from panning back and forth, make him walk an extra distance.
	local screen = rainbow.platform.screen;
	if self.position == screen.width and walk.x > screen.width - self.margin then
		walk.x = screen.width + self.margin;
	elseif self.position == 0 and walk.x < self.margin then
		walk.x = -self.margin;
	end
	threepwood:move(walk);
end

function SceneSCUMMBar:touch_canceled()
end

function SceneSCUMMBar:touch_ended()
end

function SceneSCUMMBar:touch_moved()
end

function SceneSCUMMBar:update()
	local threepwood = threepwood
	threepwood:update();

	-- Determine whether we should be panning the screen.
	local screen = rainbow.platform.screen;
	local x = threepwood:get_position();
	if x > screen.width - self.margin then
		self.scene = 0;
	elseif x < self.margin then
		self.scene = screen.width;
	end
	if self.scene ~= self.position then
		threepwood.lock = true;
		local x = 10;
		if self.scene < self.position then
			x = -10;
		end
		rainbow.scenegraph:move(self.node, x, 0);
		self.position = self.position + x;
	else
		threepwood.lock = false;
	end
end
