-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local format = string.format
local pairs = pairs
local tonumber = tonumber
local type = type

local rainbow = rainbow
local scenegraph = rainbow.scenegraph

local SoundFinalizer = {
	__gc = function(self)
		rainbow.audio.delete_sound(self)
	end
}

local function endswith(str, ending)
	return ending == '' or str:sub(-ending:len()) == ending
end

local function insert(t, k, v)
	if t[k] then
		error(format("'%s' is defined twice", k))
	end
	t[k] = v
end

local function merge(t1, t2)
	for k,v in pairs(t2) do
		insert(t1, k, v)
	end
end

local function setproperties(obj, def)
	if def.alignment then
		obj:set_alignment(def.alignment);
	end
	if def.color then
		local r = tonumber(def.color:sub(1, 2), 16)
		local g = tonumber(def.color:sub(3, 4), 16)
		local b = tonumber(def.color:sub(5, 6), 16)
		local a = tonumber(def.color:sub(7, 8), 16)
		obj:set_color(r, g, b, a)
	end
	if def.frames then
		local fps = def.frames.fps
		local delay = def.frames.delay
		def.frames.fps = nil
		def.frames.delay = nil
		local count = #def.frames
		if delay then
			obj.animation = rainbow.animation(obj, def.frames, fps, delay)
		else
			obj.animation = rainbow.animation(obj, def.frames, fps)
		end
	end
	if def.position then
		local x, y = 1, 2
		obj:set_position(def.position[x], def.position[y])
	end
	if def.rotation then
		obj:set_rotation(def.rotation)
	end
	if def.scale then
		obj:set_scale(def.scale)
	end
	if def.texture then
		obj:set_texture(def.texture)
	end
end

local function createbatch(node, resources)
	local width, height = 1, 2
	local batch = rainbow.spritebatch(#node.sprites)
	batch:set_texture(resources[node.texture])
	local sprites = {}
	for i = 1, #node.sprites do
		local def = node.sprites[i]
		local sprite = batch:create_sprite(def.size[width], def.size[height])
		setproperties(sprite, def)
		insert(sprites, def.name, sprite)
	end
	return batch, sprites
end

local function createfont(def)
	local path, size = 1, 2
	return rainbow.font(def[path], def[size])
end

local function createlabel(def, resources)
	local label = def.text and rainbow.label(def.text) or rainbow.label()
	label:set_font(resources[def.font])
	setproperties(label, def)
	return label
end

local function createsound(def)
	local sound = rainbow.audio.create_sound(def)
	return setmetatable(sound, SoundFinalizer);
end

local function createtexture(def)
	local path = 1
	local x, y, width, height = 1, 2, 3, 4
	local texture = rainbow.texture(def[path])
	for i = 2, #def do
		texture:create(def[i][x], def[i][y], def[i][width], def[i][height])
	end
	return texture
end

local function createresources(resources)
	local t = {}
	for name,resource in pairs(resources) do
		local path = type(resource) ~= "table" and resource or resource[1]
		if endswith(path, ".png") or endswith(path, ".pvr") then
			insert(t, name, createtexture(resource))
		elseif endswith(path, ".otf") or endswith(path, ".ttf") then
			insert(t, name, createfont(resource))
		elseif endswith(path, ".ogg") or endswith(path, ".mp3") then
			insert(t, name, createsound(resource))
		else
			print("Unknown resource: " .. path)
		end
	end
	return t
end

local function createnodes(parent, resources, nodes)
	local t = {}
	for i = 1, #nodes do
		local node = nodes[i]
		if node.sprites then
			local batch, sprites = createbatch(node, resources)
			merge(t, sprites)
			batch.node = scenegraph:add_batch(parent, batch)
			for _,sprite in pairs(sprites) do
				if sprite.animation then
					scenegraph:add_animation(batch.node, sprite.animation)
				end
			end
			insert(t, node.name, batch)
		elseif node.font then
			local label = createlabel(node, resources)
			label.node = scenegraph:add_label(parent, label)
			insert(t, node.name, label)
		elseif node.nodes then
			local group = scenegraph:add_node(parent)
			merge(t, createnodes(group, resources, node.nodes))
			insert(t, node.name, group)
		else
			print("Unknown element: " .. node.name)
		end
	end
	return t
end

return {
	createresources = createresources,
	createnodes = createnodes
}
