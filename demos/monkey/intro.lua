--! Monkey Demo: Intro
--!
--! Copyright 2011-12 Bifrost Entertainment. All rights reserved.
--! \author Tommy Nguyen

SceneIntro = {};
SceneIntro.__index = SceneIntro;

function SceneIntro.new(assets)
	local s = {};
	setmetatable(s, SceneIntro);

	s.spritebatch = rainbow.spritebatch(2);
	s.spritebatch:set_texture(assets);

	s.bg_island = s.spritebatch:add(0, 0, 320, 200);
	s.bg_title = nil;

	s.node = nil;
	s.sched = Scheduler.new(22);

	return s;
end

function SceneIntro:destruct()
	self.bg_island = nil;
	self.bg_title = nil;
	self.sched = nil;
	self.spritebatch = nil;
end

function SceneIntro:key_down()
end

function SceneIntro:key_up()
end

function SceneIntro:init()
	if self.node then
		return
	end

	-- Start opening theme
	rainbow.audio.set_bgm("opening.ogg");
	rainbow.audio.play_bgm();

	-- Scale and position island
	self.bg_island:set_scale(global_scale);
	local screen = rainbow.platform.screen;
	self.bg_island:set_position(screen.width * 0.5, screen.height * 0.5);

	-- Add this sprite batch to the scene graph
	self.node = rainbow.scenegraph:add_batch(self.spritebatch);

	-- Add timer to display logo
	self.sched:add(self);

	-- Enable input
	rainbow.input.subscribe(self);
end

function SceneIntro:tick()
	-- Remove all timed event subscribers
	self.sched:remove_all();

	-- Add logo
	self.bg_title = self.spritebatch:add(16, 202, 216, 120);
	self.bg_title:set_scale(global_scale);

	-- Place logo
	local screen = rainbow.platform.screen;
	local title_offset_x = global_scale * -4;
	local title_offset_y = global_scale * 16;
	self.bg_title:set_position(screen.width * 0.5 + title_offset_x, screen.height * 0.5 + title_offset_y);
end

function SceneIntro:touch_began()
	rainbow.input.unsubscribe(self);
	rainbow.audio.stop_bgm();
	rainbow.scenegraph:remove(self.node);
	self.node = nil;
end

function SceneIntro:touch_canceled()
end

function SceneIntro:touch_ended()
end

function SceneIntro:touch_moved()
end

function SceneIntro:update()
	if not self.node then
		return "scummbar";
	end
	self.sched:update();
end
