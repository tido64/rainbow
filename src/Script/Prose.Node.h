// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

template<typename T>
void set_color_and_transform(lua_State *L, T asset)
{
	if (has_key(L, "color"))
	{
		auto field = get_field(L, "color");
		asset->set_color(
		    Colorb(static_cast<unsigned int>(lua_tointeger(L, -1))));
	}
	if (has_key(L, "position"))
	{
		auto field = get_field(L, "position");
		lua_rawgeti(L, -1, 1);
		lua_rawgeti(L, -2, 2);
		asset->set_position(Vec2f(lua_tonumber(L, -2), lua_tonumber(L, -1)));
		lua_pop(L, 2);
	}
	if (has_key(L, "rotation"))
	{
		auto field = get_field(L, "rotation");
		asset->set_rotation(lua_tonumber(L, -1));
	}
	if (has_key(L, "scale"))
	{
		auto field = get_field(L, "scale");
		asset->set_scale(lua_tonumber(L, -1));
	}
}

Prose::Asset create_animation(lua_State *L,
                              const Sprite::Ref &sprite,
                              rainbow::ScopeStack &stack,
                              rainbow::SceneNode *parent)
{
	const auto table = lua_gettop(L);
	const size_t num_frames = lua_rawlen(L, table);
	Animation::Frames frames(new Animation::Frame[num_frames + 1]);
	for (size_t i = 0; i < num_frames; ++i)
	{
		lua_rawgeti(L, table, i + 1);
		frames[i] = lua_tointeger(L, -1);
	}
	lua_settop(L, table);
	frames[num_frames] = Animation::kAnimationEnd;

	unsigned int fps = 0;
	if (!has_key(L, "fps"))
		R_ABORT(kProseMissingProperty, "fps", "animation", table_name(L));
	else
	{
		auto field = get_field(L, "fps");
		fps = lua_tointeger(L, -1);
	}

	int delay = 0;
	if (has_key(L, "delay"))
	{
		auto field = get_field(L, "delay");
		delay = lua_tointeger(L, -1);
	}

	auto animation =
	    stack.allocate<Animation>(sprite, std::move(frames), fps, delay);
	auto node = parent->add_child(animation);
#if USE_NODE_TAGS
	node->set_tag(table_name(L));
#endif  // USE_NODE_TAGS
	return {Prose::AssetType::Animation, animation, node};
}

Prose::Asset create_label(lua_State *L,
                          Prose &scene,
                          rainbow::ScopeStack &stack,
                          rainbow::SceneNode *parent)
{
	auto label = stack.allocate<Label>();
	set_color_and_transform(L, label);
	if (has_key(L, "alignment"))
	{
		auto field = get_field(L, "alignment");
		Label::TextAlignment alignment = Label::TextAlignment::Left;
		const char *set = lua_tostring(L, -1);
		if (*set == 'c')
			alignment = Label::TextAlignment::Center;
		else if (*set == 'r')
			alignment = Label::TextAlignment::Right;
		label->set_alignment(alignment);
	}
	if (has_key(L, "font"))
	{
		auto field = get_field(L, "font");
		label->set_font(SharedPtr<FontAtlas>(
		    scene.get_asset<FontAtlas>(lua_tostring(L, -1))));
	}
	if (has_key(L, "text"))
	{
		auto field = get_field(L, "text");
		label->set_text(lua_tostring(L, -1));
	}
	return {Prose::AssetType::Label, label, parent->add_child(label)};
}

Prose::Asset create_sprite(lua_State *L,
                           SpriteBatch *batch,
                           rainbow::ScopeStack &stack,
                           rainbow::SceneNode *parent)
{
	Sprite::Ref sprite;
	if (!has_key(L, "size"))
		R_ABORT(kProseMissingProperty, "size", "sprite", table_name(L));
	else
	{
		auto field = get_field(L, "size");
		lua_rawgeti(L, -1, 1);
		lua_rawgeti(L, -2, 2);
		sprite =
		    batch->create_sprite(lua_tointeger(L, -2), lua_tointeger(L, -1));
		lua_pop(L, 2);
	}
	if (!has_key(L, "texture"))
		R_ABORT(kProseMissingProperty, "texture", "sprite", table_name(L));
	else
	{
		auto field = get_field(L, "texture");
		sprite->set_texture(lua_tointeger(L, -1));
	}
	set_color_and_transform(L, sprite);
	if (has_key(L, "normal"))
	{
		auto field = get_field(L, "normal");
		sprite->set_normal(lua_tointeger(L, -1));
	}
	if (has_key(L, "pivot"))
	{
		auto field = get_field(L, "pivot");
		lua_rawgeti(L, -1, 1);
		lua_rawgeti(L, -2, 2);
		sprite->set_pivot(Vec2f(lua_tonumber(L, -2), lua_tonumber(L, -1)));
		lua_pop(L, 2);
	}
	if (has_key(L, "animations"))
		parse_table(L, "animations", &create_animation, sprite, stack, parent);
	return {Prose::AssetType::Sprite, nullptr, nullptr};
}

Prose::Asset create_spritebatch(lua_State *L,
                                Prose &scene,
                                rainbow::ScopeStack &stack,
                                rainbow::SceneNode *parent)
{
	auto batch = stack.allocate<SpriteBatch>();
	auto field = get_field(L, "texture");
	batch->set_texture(SharedPtr<TextureAtlas>(
	    scene.get_asset<TextureAtlas>(lua_tostring(L, -1))));
	return {Prose::AssetType::SpriteBatch, batch, parent->add_child(batch)};
}

Prose::AssetType node_type(lua_State *L)
{
	if (has_key(L, "sprites"))
		return Prose::AssetType::SpriteBatch;
	if (has_key(L, "font"))
		return Prose::AssetType::Label;
	if (has_key(L, "nodes"))
		return Prose::AssetType::Node;
	return Prose::AssetType::None;
}

Prose::Asset create_node(lua_State *L,
                         Prose &scene,
                         Prose::AssetMap &assets,
                         rainbow::ScopeStack &stack,
                         rainbow::SceneNode *parent)
{
	Prose::Asset asset = no_asset();
	switch (node_type(L))
	{
		case Prose::AssetType::Label:
			asset = create_label(L, scene, stack, parent);
			break;
		case Prose::AssetType::Node:
			asset = {Prose::AssetType::Node,
			         nullptr,
			         parent->add_child(new rainbow::SceneNode())};
			break;
		case Prose::AssetType::SpriteBatch: {
			asset = create_spritebatch(L, scene, stack, parent);
			if (has_key(L, "sprites"))
			{
				parse_table(L,
				            "sprites",
				            &create_sprite,
				            static_cast<SpriteBatch*>(asset.ptr),
				            stack,
				            asset.node);
			}
			break;
		}
		default:
			break;
	}
	if (asset.type != Prose::AssetType::None)
	{
		const char *name = table_name(L);
#if USE_NODE_TAGS
		asset.node->set_tag(name);
#endif  // USE_NODE_TAGS
		assets[name] = asset;
		if (asset.type == Prose::AssetType::Node || has_key(L, "nodes"))
		{
			parse_table(L,
			            "nodes",
			            &create_node,
			            scene,
			            assets,
			            stack,
			            asset.node);
		}
	}
	return asset;
}
