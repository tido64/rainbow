// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

auto create_font(lua_State* L, ScopeStack& stack) -> Prose::Asset
{
    lua_rawgeti(L, -1, 1);
    const char* path = lua_tostring(L, -1);
    lua_rawgeti(L, -2, 2);
    const float pt = lua_tonumber(L, -1);
    lua_pop(L, 2);
    if (pt < 1.0f)
    {
        LOGE("Prose: Invalid size for font: %s", path);
        return Prose::Asset::none();
    }

    const Data& data = Data::load_asset(path);
    if (!data)
    {
        LOGE(kProseFailedOpening, path);
        return Prose::Asset::none();
    }

    auto font = stack.allocate<FontAtlas>(data, pt);
    if (!font->is_valid())
    {
        LOGE(kProseFailedLoading, kKeyFont, path);
        return Prose::Asset::none();
    }

    return {Prose::AssetType::FontAtlas, font, 0};
}

auto create_sound(lua_State*, ScopeStack&) -> Prose::Asset
{
    LOGW("Prose: Sound is not yet implemented");
    return {Prose::AssetType::Sound, nullptr, 0};
}

auto create_texture(lua_State* L, ScopeStack& stack) -> Prose::Asset
{
    const int top = lua_gettop(L);
    lua_pushnil(L);
    if (lua_next(L, -2) == 0 || lua_type(L, -1) != LUA_TSTRING)
    {
        lua_settop(L, top);
        return Prose::Asset::none();
    }

    const auto path = rainbow::filesystem::relative(lua_tostring(L, -1));
    lua_pop(L, 1);
    std::error_code error;
    if (!rainbow::filesystem::is_regular_file(path, error))
    {
        LOGE(kProseNoSuchFile, path.c_str());
        lua_pop(L, 1);
        return Prose::Asset::none();
    }

    auto texture = stack.allocate<TextureAtlas>(path);
    if (!texture->is_valid())
    {
        LOGE(kProseFailedLoading, kKeyTexture, path.c_str());
        lua_pop(L, 1);
        return Prose::Asset::none();
    }

    stack.retain(texture);
    while (lua_next(L, -2) != 0)
    {
        if (!lua_istable(L, -1))
        {
            LOGW(kProseUnknownProperty,
                 table_name(L),
                 kKeyTexture,
                 path.c_str());
            lua_pop(L, 1);
            continue;
        }
        lua_rawgeti(L, -1, 1);
        const int x = lua_tointeger(L, -1);
        lua_rawgeti(L, -2, 2);
        const int y = lua_tointeger(L, -1);
        lua_rawgeti(L, -3, 3);
        const int width = lua_tointeger(L, -1);
        lua_rawgeti(L, -4, 4);
        const int height = lua_tointeger(L, -1);
        lua_pop(L, 5);
        texture->add_region(x, y, width, height);
    }
    return {Prose::AssetType::TextureAtlas, texture, 0};
}

bool ends_with(const char* str, const char* suffix)
{
    const auto str_length = strlen(str);
    const auto suffix_length = strlen(suffix);
    if (suffix_length > str_length)
        return false;

    const char* s = str + str_length - suffix_length;
    for (size_t i = 0; i < suffix_length; ++i, ++s)
        if (tolower(*s) != tolower(suffix[i]))
            return false;

    return true;
}

auto resource_type(lua_State* L) -> Prose::AssetType
{
    const auto field = lua_type(L, -1);
    if (field == LUA_TSTRING)
    {
        const char* file = lua_tostring(L, -1);
        return (!ends_with(file, ".mp3") && !ends_with(file, ".ogg")
                    ? Prose::AssetType::None
                    : Prose::AssetType::Sound);
    }
    else if (field == LUA_TTABLE)
    {
        ScopedField field(L, 1);
        if (lua_type(L, -1) != LUA_TSTRING)
            return Prose::AssetType::None;
        const char* file = lua_tostring(L, -1);
        if (ends_with(file, ".png") || ends_with(file, ".pvr"))
            return Prose::AssetType::TextureAtlas;
        if (ends_with(file, ".otf") || ends_with(file, ".ttf"))
            return Prose::AssetType::FontAtlas;
    }
    return Prose::AssetType::None;
}

auto create_resource(lua_State* L,
                     ScopeStack& stack,
                     Prose::AssetMap& assets) -> Prose::Asset
{
    auto asset = Prose::Asset::none();
    switch (resource_type(L))
    {
        case Prose::AssetType::FontAtlas:
            asset = create_font(L, stack);
            break;
        case Prose::AssetType::Sound:
            asset = create_sound(L, stack);
            break;
        case Prose::AssetType::TextureAtlas:
            asset = create_texture(L, stack);
            break;
        default:
            return asset;
    }

    assets[table_name(L)] = asset;
    return asset;
}
