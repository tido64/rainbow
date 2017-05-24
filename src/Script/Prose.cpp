// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Prose.h"

#include <string>

#include "Common/Data.h"
#include "Common/String.h"
#include "FileSystem/File.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

#define kProseFailedLoading    "Prose: Failed to load %s: %s"
#define kProseFailedOpening    "Prose: Failed to open file: %s"
#define kProseMissingProperty  "Prose: Missing property '%s' on %s: %s"
#define kProseUnknownProperty  "Prose: Unknown property '%s' on %s: %s"

using rainbow::Animation;
using rainbow::Color;
using rainbow::FontAtlas;
using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::Label;
using rainbow::Prose;
using rainbow::ScopeStack;
using rainbow::SharedPtr;
using rainbow::Sprite;
using rainbow::SpriteRef;
using rainbow::SpriteBatch;
using rainbow::TextAlignment;
using rainbow::TextureAtlas;
using rainbow::Vec2f;
using rainbow::czstring;
using rainbow::graphics::RenderQueue;

namespace
{
    constexpr char kKeyAlignment[] = "alignment";
    constexpr char kKeyAnimations[] = "animations";
    constexpr char kKeyColor[] = "color";
    constexpr char kKeyDelay[] = "delay";
    constexpr char kKeyFont[] = "font";
    constexpr char kKeyFps[] = "fps";
    constexpr char kKeyName[] = "name";
    constexpr char kKeyNodes[] = "nodes";
    constexpr char kKeyNormal[] = "normal";
    constexpr char kKeyPivot[] = "pivot";
    constexpr char kKeyPosition[] = "position";
    constexpr char kKeyResources[] = "resources";
    constexpr char kKeyRotation[] = "rotation";
    constexpr char kKeyScale[] = "scale";
    constexpr char kKeySize[] = "size";
    constexpr char kKeySprites[] = "sprites";
    constexpr char kKeyText[] = "text";
    constexpr char kKeyTexture[] = "texture";
}

enum class Prose::AssetType
{
    None,
    Animation,
    FontAtlas,
    Label,
    Node,
    Sound,
    Sprite,
    SpriteBatch,
    TextureAtlas
};

auto Prose::Asset::none() -> Prose::Asset
{
    return {AssetType::None, nullptr, 0};
}

Prose::Prose(size_t size) : allocator_(size), stack_(allocator_)
{
}

template <typename T, Prose::AssetType Type>
auto Prose::get_asset(const std::string& name, czstring type_name) -> T*
{
    auto asset = assets_.find(name);
    if (asset == assets_.end() || asset->second.type != Type)
    {
        R_ABORT("Prose: No such %s: %s", type_name, name.c_str());
        NOT_USED(type_name);
        return nullptr;
    }

    return static_cast<T*>(asset->second.ptr);
}

// Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56480
#if defined(__GNUC__) && !defined(__clang__)
namespace rainbow {
#endif

template <>
auto Prose::get_asset<Animation>(const std::string& name) -> Animation*
{
    return get_asset<Animation, AssetType::Animation>(name, "animation");
}

template <>
auto Prose::get_asset<FontAtlas>(const std::string& name) -> FontAtlas*
{
    return get_asset<FontAtlas, AssetType::FontAtlas>(name, "font");
}

template <>
auto Prose::get_asset<Label>(const std::string& name) -> Label*
{
    return get_asset<Label, AssetType::Label>(name, "label");
}

template <>
auto Prose::get_asset<Sprite>(const std::string& name) -> Sprite*
{
    return get_asset<Sprite, AssetType::Sprite>(name, "sprite");
}

template <>
auto Prose::get_asset<SpriteBatch>(const std::string& name) -> SpriteBatch*
{
    return get_asset<SpriteBatch, AssetType::SpriteBatch>(name, "sprite batch");
}

template <>
auto Prose::get_asset<TextureAtlas>(const std::string& name) -> TextureAtlas*
{
    return get_asset<TextureAtlas, AssetType::TextureAtlas>(name, "texture");
}

#if defined(__GNUC__) && !defined(__clang__)
}  // Workaround for https://gcc.gnu.org/bugzilla/show_bug.cgi?id=56480
#endif

auto Prose::get_animation(const std::string& name) -> Animation*
{
    return get_asset<Animation>(name);
}

auto Prose::get_font(const std::string& name) -> FontAtlas*
{
    return get_asset<FontAtlas>(name);
}

auto Prose::get_label(const std::string& name) -> Label*
{
    return get_asset<Label>(name);
}

auto Prose::get_sprite(const std::string& name) -> Sprite*
{
    return get_asset<Sprite>(name);
}

auto Prose::get_spritebatch(const std::string& name) -> SpriteBatch*
{
    return get_asset<SpriteBatch>(name);
}

auto Prose::get_texture(const std::string& name) -> TextureAtlas*
{
    return get_asset<TextureAtlas>(name);
}

namespace
{
    constexpr int kProseVersion = 100;

    class ScopedField
    {
    public:
        ScopedField(lua_State* L, int n) : state_(L) { lua_rawgeti(L, -1, n); }

        ScopedField(lua_State* L, czstring s) : state_(L)
        {
            lua_pushstring(L, s);
            lua_rawget(L, -2);
        }

        ScopedField(lua_State* L, czstring s, size_t len) : state_(L)
        {
            lua_pushlstring(L, s, len);
            lua_rawget(L, -2);
        }

        ~ScopedField() { lua_pop(state_, 1); }

    private:
        lua_State* state_;
    };

    template <size_t N>
    auto get_field(lua_State* L, const char (&name)[N])
    {
        return ScopedField{L, name, N - 1};
    }

    template <size_t N>
    bool has_key(lua_State* L, const char (&name)[N])
    {
        if (!lua_istable(L, -1))
            return false;

        auto field = get_field(L, name);
        return !lua_isnil(L, -1);
    }

    auto table_length(lua_State *L, czstring name)
    {
        ScopedField table{L, name};
        R_ASSERT(lua_istable(L, -1), "Table expected");

        uint32_t length = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            ++length;
            lua_pop(L, 1);
        }
        return length;
    }

    auto table_name(lua_State* L) -> czstring
    {
        if (has_key(L, kKeyName))
        {
            auto field = get_field(L, kKeyName);
            return lua_tostring(L, -1);
        }

        switch (lua_type(L, -2))
        {
            case LUA_TNUMBER: {
                czstring name = lua_pushfstring(L, "#%d", lua_tointeger(L, -2));
                lua_pop(L, 1);
                return name;
            }
            case LUA_TSTRING:
                return lua_tostring(L, -2);
            default:
                break;
        }

        return "";
    }

    template <typename F, typename... Args>
    void parse_table(lua_State* L, czstring name, F&& parse, Args&&... args)
    {
        ScopedField table{L, name};
        R_ASSERT(lua_istable(L, -1), "Table expected");

        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            const auto asset = parse(L, std::forward<Args>(args)...);
            if (asset.type == Prose::AssetType::None)
                LOGE("Prose: Failed to parse '%s'", table_name(L));
            lua_pop(L, 1);
        }
    }

#include "Script/Prose.Node.h"
#include "Script/Prose.Resource.h"

    auto size_of(Prose::AssetType type) -> size_t
    {
        switch (type)
        {
            case Prose::AssetType::Animation:
                return ScopeStack::size_of<Animation>();
            case Prose::AssetType::FontAtlas:
                return ScopeStack::size_of<FontAtlas>();
            case Prose::AssetType::Label:
                return ScopeStack::size_of<Label>();
            case Prose::AssetType::SpriteBatch:
                return ScopeStack::size_of<SpriteBatch>();
            case Prose::AssetType::Sound:
                return ScopeStack::size_of<void*>();
            case Prose::AssetType::TextureAtlas:
                return ScopeStack::size_of<TextureAtlas>();
            default:
                return 0;
        }
    }

    auto compute_size(lua_State* L, size_t& total_size) -> Prose::Asset
    {
        auto type = node_type(L);
        if (type == Prose::AssetType::None)
            type = resource_type(L);
        else if (type == Prose::AssetType::SpriteBatch)
            parse_table(L, kKeySprites, &compute_size, total_size);
        if (type == Prose::AssetType::None)
        {
            if (has_key(L, kKeyTexture))
            {
                type = Prose::AssetType::Sprite;
                if (has_key(L, kKeyAnimations))
                    parse_table(L, kKeyAnimations, &compute_size, total_size);
            }
            else if (has_key(L, kKeyFps))
                type = Prose::AssetType::Animation;
        }
        total_size += size_of(type);
        return {type, nullptr, 0};
    }
}

auto Prose::from_lua(czstring path) -> Prose*
{
    const Data& script = File::read(path, FileType::Asset);
    if (!script)
        return nullptr;

    auto L = rainbow::lua::newstate();
    lua_gc(L.get(), LUA_GCSTOP, 0);
    lua_createtable(L.get(), 0, 1);
    lua_pushliteral(L.get(), "platform");
    lua_createtable(L.get(), 0, 1);
    lua_pushliteral(L.get(), "screen");
    lua_createtable(L.get(), 0, 2);
    const Vec2i& screen = rainbow::graphics::resolution();
    luaR_rawsetnumber(L.get(), "width", screen.x);
    luaR_rawsetnumber(L.get(), "height", screen.y);
    lua_rawset(L.get(), -3);
    lua_rawset(L.get(), -3);
    lua_setglobal(L.get(), "rainbow");

    if (rainbow::lua::load(L.get(), script, "Prose") == 0)
        return nullptr;

    if (!has_key(L.get(), "version"))
        R_ABORT("Prose: Version required and missing");
    else
    {
        auto field = get_field(L.get(), "version");
        if (lua_tointeger(L.get(), -1) < kProseVersion)
        {
            R_ABORT("Prose: Version %i required", kProseVersion);
            return nullptr;
        }
    }

    // Compute the memory requirement for this scene.
    size_t total_size = 0;
    parse_table(L.get(), kKeyResources, &compute_size, total_size);
    parse_table(L.get(), kKeyNodes, &compute_size, total_size);

    auto scene = std::make_unique<Prose>(total_size);
    parse_table(
        L.get(), kKeyResources, &create_resource, scene->stack_, scene->assets_);
    parse_table(L.get(),
                kKeyNodes,
                &create_node,
                *scene,
                scene->assets_,
                scene->stack_,
                scene->render_queue_);
    return scene.release();
}
