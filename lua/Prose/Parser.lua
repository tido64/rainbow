-- Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
-- Distributed under the MIT License.
-- (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

local module_path = ((...):match("(.*[./\\])[^./\\]+") or "") .. "../"

local AnimationController = require(module_path .. "AnimationController")
local F = require(module_path .. "Functional")

local format = string.format
local pairs = pairs
local tonumber = tonumber
local type = type

local rainbow = rainbow
local scenegraph = rainbow.scenegraph

--
-- Helper functions
--

local function extend(data)
	return setmetatable(
		{ __userdata = data },
		{ __index = function(self, key)
			local field = self.__userdata[key]
			if not field then
				return nil
			end
			self[key] = function(self, ...)
				return field(self.__userdata, ...)
			end
			return self[key]
		end })
end

local function insert(t, k, v)
	t[k] = t[k] and error(format("'%s' is defined twice", k)) or v
end

local function copy_into(t1, t2)
	for k,v in pairs(t2) do
		insert(t1, k, v)
	end
end

local function set_properties(obj, def)
	if def.alignment then
		obj:set_alignment(def.alignment)
	end
	if def.color then
		local r = tonumber(def.color:sub(1, 2), 16)
		local g = tonumber(def.color:sub(3, 4), 16)
		local b = tonumber(def.color:sub(5, 6), 16)
		local a = tonumber(def.color:sub(7, 8), 16)
		obj:set_color(r, g, b, a)
	end
	if def.pivot then
		local x, y = 1, 2
		obj:set_pivot(def.pivot[x], def.pivot[y])
	end
	if def.position then
		local x, y = 1, 2
		obj:set_position(def.position[x], def.position[y])
	end
	if def.rotation then
		obj:set_rotation(def.rotation)
	end
	if def.scale then
		if type(def.scale) == "table" then
			local x, y = 1, 2
			obj:set_scale(def.scale[x], def.scale[y])
		else
			obj:set_scale(def.scale)
		end
	end
	if def.texture then
		obj:set_texture(def.texture)
	end
end

--
-- Drawables
--

local function create_batch(node, resources)
	local width, height = 1, 2
	local batch = extend(rainbow.spritebatch(#node.sprites))
	batch:set_texture(resources[node.texture])
	local f = function(t, def)
		local sprite =
		    extend(batch:create_sprite(def.size[width], def.size[height]))
		set_properties(sprite, def)
		if def.animations then
			local f = function(_, v)
				local a = rainbow.animation(sprite, v, v.fps, v.delay or 0)
				a:stop()
				return a
			end
			sprite.animations = AnimationController(F.maph(f, def.animations))
		end
		insert(t, def.name, sprite)
		return t
	end
	return batch, F.foldl(f, {}, node.sprites)
end

local function create_label(def, resources)
	local label = extend(rainbow.label(def.text or nil))
	label:set_font(resources[def.font])
	set_properties(label, def)
	return label
end

--
-- Resources
--

local ResourceFactory
do
	local audio = rainbow.audio
	local FMOD = FMOD

	local function create_font(def)
		local path, size = 1, 2
		return rainbow.font(def[path], def[size])
	end

	local create_sound =
	    FMOD and function(snd)
	                 return FMOD.createSound(snd)
	             end
	          or function(snd)
	                 return setmetatable(audio.create_sound(snd), {
	                     __gc = function(self) audio.delete_sound(self) end
	                 })
	             end

	local function create_texture(def)
		local path = 1
		local x, y, width, height = 1, 2, 3, 4
		local texture = rainbow.texture(def[path])
		for i = 2, #def do
			local t = def[i]
			texture:create(t[x], t[y], t[width], t[height])
		end
		return texture
	end

	ResourceFactory = {
		mp3 = create_sound,
		ogg = create_sound,
		otf = create_font,
		png = create_texture,
		pvr = create_texture,
		ttf = create_font,
	}
end

local function create_resource(name, resource)
	local path = type(resource) ~= "table" and resource or resource[1]
	local produce = ResourceFactory[path:sub(-3)]
	if not produce then
		print("Unknown resource: " .. path)
		return
	end
	return produce(resource)
end

--
-- Public functions
--

local function create_resources(resources)
	return F.maph(create_resource, resources)
end

local function create_nodes(parent, resources, nodes)
	local f = function(t, node)
		if node.sprites then
			local batch, sprites = create_batch(node, resources)
			copy_into(t, sprites)
			batch.node = scenegraph:add_batch(parent, batch)
			for _,sprite in pairs(sprites) do
				if sprite.animations then
					sprite.animations.parent = batch.node
				end
			end
			scenegraph:set_tag(batch.node, node.name)
			insert(t, node.name, batch)
		elseif node.font then
			local label = create_label(node, resources)
			label.node = scenegraph:add_label(parent, label)
			scenegraph:set_tag(label.node, node.name)
			insert(t, node.name, label)
		elseif node.nodes then
			local group = scenegraph:add_node(parent)
			copy_into(t, create_nodes(group, resources, node.nodes))
			scenegraph:set_tag(group, node.name)
			insert(t, node.name, group)
		else
			print("Unknown element: " .. node.name)
		end
		return t
	end
	return F.foldl(f, {}, nodes)
end

return {
	create_resources = create_resources,
	create_nodes = create_nodes,
	version = 1.1
}
