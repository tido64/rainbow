// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

template <typename T>
void set_color_and_transform(lua_State* L, T asset)
{
    if (has_key(L, kKeyColor))
    {
        auto field = get_field(L, kKeyColor);
        asset->set_color(Color{static_cast<uint8_t>(lua_tointeger(L, -1))});
    }
    if (has_key(L, kKeyPosition))
    {
        auto field = get_field(L, kKeyPosition);
        lua_rawgeti(L, -1, 1);
        lua_rawgeti(L, -2, 2);
        asset->set_position(Vec2f{lua_tonumber(L, -2), lua_tonumber(L, -1)});
        lua_pop(L, 2);
    }
    if (has_key(L, kKeyRotation))
    {
        auto field = get_field(L, kKeyRotation);
        asset->set_rotation(lua_tonumber(L, -1));
    }
    if (has_key(L, kKeyScale))
    {
        auto field = get_field(L, kKeyScale);
        asset->set_scale(lua_tonumber(L, -1));
    }
}

auto create_animation(lua_State* L,
                      const SpriteRef& sprite,
                      ScopeStack& stack,
                      RenderQueue& render_queue) -> Prose::Asset
{
    const auto table = lua_gettop(L);
    const auto num_frames = static_cast<int>(lua_rawlen(L, table));
    auto frames = std::make_unique<Animation::Frame[]>(num_frames + 1);
    for (int i = 0; i < num_frames; ++i)
    {
        lua_rawgeti(L, table, i + 1);
        frames[i] = lua_tointeger(L, -1);
    }
    lua_settop(L, table);
    frames[num_frames] = Animation::kAnimationEnd;

    unsigned int fps = 0;
    if (!has_key(L, kKeyFps))
        R_ABORT(kProseMissingProperty, kKeyFps, kKeyAnimation, table_name(L));
    else
    {
        auto field = get_field(L, kKeyFps);
        fps = lua_tointeger(L, -1);
    }

    int delay = 0;
    if (has_key(L, kKeyDelay))
    {
        auto field = get_field(L, kKeyDelay);
        delay = lua_tointeger(L, -1);
    }

    auto animation = stack.allocate<Animation>(
        sprite,
        Animation::Frames(
            // The cast is necessary for Visual Studio 2013.
            static_cast<const Animation::Frame*>(frames.release())),
        fps,
        delay);
    render_queue.emplace_back(*animation, table_name(L));
    const auto id = static_cast<uint32_t>(render_queue.size() - 1);
    return {Prose::AssetType::Animation, animation, id};
}

auto create_label(lua_State* L,
                  Prose& scene,
                  ScopeStack& stack,
                  RenderQueue& render_queue) -> Prose::Asset
{
    auto label = stack.allocate<Label>();
    set_color_and_transform(L, label);
    if (has_key(L, kKeyAlignment))
    {
        auto field = get_field(L, kKeyAlignment);
        const TextAlignment alignment =
            rainbow::text_alignment_from_char(*lua_tostring(L, -1));
        label->set_alignment(alignment);
    }
    if (has_key(L, kKeyFont))
    {
        auto field = get_field(L, kKeyFont);
        label->set_font(SharedPtr<FontAtlas>(
            scene.get_asset<FontAtlas>(lua_tostring(L, -1))));
    }
    if (has_key(L, kKeyText))
    {
        auto field = get_field(L, kKeyText);
        label->set_text(lua_tostring(L, -1));
    }
    render_queue.emplace_back(*label);
    const auto id = static_cast<uint32_t>(render_queue.size() - 1);
    return {Prose::AssetType::Label, label, id};
}

auto create_sprite(lua_State* L,
                   SpriteBatch* batch,
                   ScopeStack& stack,
                   RenderQueue& render_queue) -> Prose::Asset
{
    SpriteRef sprite;
    if (!has_key(L, kKeySize))
        R_ABORT(kProseMissingProperty, kKeySize, kKeySprite, table_name(L));
    else
    {
        auto field = get_field(L, kKeySize);
        lua_rawgeti(L, -1, 1);
        lua_rawgeti(L, -2, 2);
        sprite =
            batch->create_sprite(lua_tointeger(L, -2), lua_tointeger(L, -1));
        lua_pop(L, 2);
    }
    if (!has_key(L, kKeyTexture))
        R_ABORT(kProseMissingProperty, kKeyTexture, kKeySprite, table_name(L));
    else
    {
        auto field = get_field(L, kKeyTexture);
        sprite->set_texture(lua_tointeger(L, -1));
    }
    set_color_and_transform(L, sprite);
    if (has_key(L, kKeyNormal))
    {
        auto field = get_field(L, kKeyNormal);
        sprite->set_normal(lua_tointeger(L, -1));
    }
    if (has_key(L, kKeyPivot))
    {
        auto field = get_field(L, kKeyPivot);
        lua_rawgeti(L, -1, 1);
        lua_rawgeti(L, -2, 2);
        sprite->set_pivot(Vec2f(lua_tonumber(L, -2), lua_tonumber(L, -1)));
        lua_pop(L, 2);
    }
    if (has_key(L, kKeyAnimations))
    {
        parse_table(
            L, kKeyAnimations, &create_animation, sprite, stack, render_queue);
    }
    return {Prose::AssetType::Sprite, nullptr, 0};
}

auto create_spritebatch(lua_State* L,
                        Prose& scene,
                        ScopeStack& stack,
                        uint32_t count,
                        RenderQueue& render_queue) -> Prose::Asset
{
    auto batch = stack.allocate<SpriteBatch>(count);
    auto field = get_field(L, kKeyTexture);
    batch->set_texture(SharedPtr<TextureAtlas>{
        scene.get_asset<TextureAtlas>(lua_tostring(L, -1))});
    render_queue.emplace_back(*batch);
    const auto id = static_cast<uint32_t>(render_queue.size() - 1);
    return {Prose::AssetType::SpriteBatch, batch, id};
}

auto node_type(lua_State* L)
{
    if (has_key(L, kKeySprites))
        return Prose::AssetType::SpriteBatch;
    if (has_key(L, kKeyFont))
        return Prose::AssetType::Label;
    if (has_key(L, kKeyNodes))
        return Prose::AssetType::Node;

    return Prose::AssetType::None;
}

auto create_node(lua_State* L,
                 Prose& scene,
                 Prose::AssetMap& assets,
                 ScopeStack& stack,
                 RenderQueue& render_queue) -> Prose::Asset
{
    auto asset = Prose::Asset::none();
    switch (node_type(L))
    {
        case Prose::AssetType::Label:
            asset = create_label(L, scene, stack, render_queue);
            break;
        case Prose::AssetType::Node:
            asset.type = Prose::AssetType::Node;
            break;
        case Prose::AssetType::SpriteBatch:
            if (!has_key(L, kKeySprites))
                return asset;
            asset = create_spritebatch(
                L, scene, stack, table_length(L, kKeySprites), render_queue);
            parse_table(L,
                        kKeySprites,
                        &create_sprite,
                        static_cast<SpriteBatch*>(asset.ptr),
                        stack,
                        render_queue);
            break;
        default:
            return asset;
    }

    czstring name = table_name(L);
    if (asset.type != Prose::AssetType::Node)
    {
        render_queue[asset.id].set_tag(name);
        assets[name] = asset;
    }
    if (asset.type == Prose::AssetType::Node || has_key(L, kKeyNodes))
    {
        parse_table(
            L, kKeyNodes, &create_node, scene, assets, stack, render_queue);
    }
    return asset;
}
