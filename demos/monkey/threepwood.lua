--! Monkey Demo: Guybrush Threepwood
--!
--! Copyright 2011 Bifrost Games. All rights reserved.
--! \author Tommy Nguyen

Threepwood = {};
Threepwood.__index = Threepwood;

function Threepwood.new(assets)
	local guybrush = {};
	setmetatable(guybrush, Threepwood);

	-- Load Guybrush standing, facing left or right
	guybrush.standingr = assets:create(400, 875, 104, 149);
	guybrush.standingl = assets:create(504, 875, 104, 149);

	-- Load Guybrush walking, facing left or right
	guybrush.walkingl = {};
	guybrush.walkingr = {};
	local x = 400;
	for i = 1,6 do
		guybrush.walkingl[i] = assets:create(x, 724, 104, 149);
		guybrush.walkingr[i] = assets:create(x, 575, 104, 149);
		x = x + 104;
	end
	guybrush.walkingl = rainbow.animation(nil, guybrush.walkingl, 7);
	guybrush.walkingr = rainbow.animation(nil, guybrush.walkingr, 7);

	-- Variables for controlling animations
	guybrush.walk = nil;
	guybrush.walking = nil;
	guybrush.direction = nil;
	guybrush.face = 1;
	guybrush.lock = false;

	return guybrush;
end

function Threepwood:deinit()
	rainbow.scenegraph:remove(self.parent_node, self.node);
	rainbow.input.unsubscribe(self);
end

function Threepwood:get_position()
	return self.x, self.y;
end

function Threepwood:init(parent, spritebatch, x, y)
	-- Add Guybrush to the sprite batch.
	self.sprite = spritebatch:add(400, 875, 104, 149);
	self.x = x;
	self.y = y;
	self.sprite:set_position(self.x, self.y);

	-- Bind the animation to the sprite.
	self.walkingl:set_sprite(self.sprite);
	self.walkingr:set_sprite(self.sprite);

	self.parent_node = parent;
end

function Threepwood:move(pos)
	if self.lock then
		return
	end

	-- Determine the direction Guybrush is facing and start the walking animation.
	rainbow.scenegraph:remove(self.parent_node, self.animation_node);
	self.walk = pos;
	if self.x < self.walk.x then
		self.direction = self.standingr;
		self.face = 1;
		self.walking = self.walkingr;
	else
		self.direction = self.standingl;
		self.face = -1;
		self.walking = self.walkingl;
	end
	self.animation_node = rainbow.scenegraph:add_animation(self.parent_node, self.walking);
end

function Threepwood:stop()
	rainbow.scenegraph:remove(self.parent_node, self.animation_node);
	self.sprite:set_texture(self.direction);
	self.walk = nil;
end

function Threepwood:update()
	if self.lock then
		-- While Guybrush is locked, get his screen position
		self.x, self.y = self.sprite:get_position();
	elseif self.walk then
		-- While Guybrush is walking check for when he reaches his destination
		-- and stop the animation.
		local diff_x = global_scale * self.face;
		self.x = self.x + diff_x;
		if self.face > 0 and self.x >= self.walk.x or self.face < 0 and self.x <= self.walk.x then
			diff_x = diff_x - (self.x - self.walk.x);
			self.x = self.walk.x;
			self.walk = nil;
			self:stop();
		end
		self.sprite:move(diff_x, 0);
	end
end
