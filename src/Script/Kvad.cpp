// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/Kvad.h"

#include <cctype>
#include <cstring>

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SceneGraph.h"
#include "Graphics/SpriteBatch.h"
#include "Lua/LuaHelper.h"
#include "Lua/LuaSyntax.h"

using rainbow::Kvad;
using rainbow::SceneNode;

enum class Kvad::AssetType
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

Kvad::Kvad(size_t size)
    : allocator_(size), stack_(allocator_), node_(SceneNode::create().release())
{
}

Kvad::~Kvad() { node_->remove(); }

template <typename T, Kvad::AssetType Type>
auto Kvad::get_asset(const std::string& name) -> T*
{
	auto asset = assets_.find(name);
	return (asset == assets_.end() || asset->second.type != Type
	            ? nullptr
	            : static_cast<T*>(asset->second.ptr));
}

template <>
auto Kvad::get_asset<Animation>(const std::string& name) -> Animation*
{
	auto animation = get_asset<Animation, AssetType::Animation>(name);
	if (!animation)
		R_ABORT("Kvad: No such animation: %s", name.c_str());
	return animation;
}

template <>
auto Kvad::get_asset<FontAtlas>(const std::string& name) -> FontAtlas*
{
	auto font = get_asset<FontAtlas, AssetType::FontAtlas>(name);
	if (!font)
		R_ABORT("Kvad: No such font: %s", name.c_str());
	return font;
}

template <>
auto Kvad::get_asset<Label>(const std::string& name) -> Label*
{
	auto label = get_asset<Label, AssetType::Label>(name);
	if (!label)
		R_ABORT("Kvad: No such label: %s", name.c_str());
	return label;
}

template <>
auto Kvad::get_asset<SceneNode>(const std::string& name) -> SceneNode*
{
	auto node = get_asset<SceneNode, AssetType::Node>(name);
	if (!node)
		R_ABORT("Kvad: No such node: %s", name.c_str());
	return node;
}

template <>
auto Kvad::get_asset<Sprite>(const std::string& name) -> Sprite*
{
	auto sprite = get_asset<Sprite, AssetType::Sprite>(name);
	if (!sprite)
		R_ABORT("Kvad: No such sprite: %s", name.c_str());
	return sprite;
}

template <>
auto Kvad::get_asset<SpriteBatch>(const std::string& name) -> SpriteBatch*
{
	auto batch = get_asset<SpriteBatch, AssetType::SpriteBatch>(name);
	if (!batch)
		R_ABORT("Kvad: No such sprite batch: %s", name.c_str());
	return batch;
}

template <>
auto Kvad::get_asset<TextureAtlas>(const std::string& name) -> TextureAtlas*
{
	auto texture = get_asset<TextureAtlas, AssetType::TextureAtlas>(name);
	if (!texture)
		R_ABORT("Kvad: No such texture: %s", name.c_str());
	return texture;
}

auto Kvad::get_animation(const std::string& name) -> Animation*
{
	return get_asset<Animation>(name);
}

auto Kvad::get_font(const std::string& name) -> FontAtlas*
{
	return get_asset<FontAtlas>(name);
}

auto Kvad::get_label(const std::string& name) -> Label*
{
	return get_asset<Label>(name);
}

auto Kvad::get_node(const std::string& name) -> SceneNode*
{
	auto asset = assets_.find(name);
	if (asset == assets_.end())
	{
		R_ABORT("Kvad: No such node: %s", name.c_str());
		return nullptr;
	}
	return asset->second.node;
}

auto Kvad::get_sprite(const std::string& name) -> Sprite*
{
	return get_asset<Sprite>(name);
}

auto Kvad::get_spritebatch(const std::string& name) -> SpriteBatch*
{
	return get_asset<SpriteBatch>(name);
}

auto Kvad::get_texture(const std::string& name) -> TextureAtlas*
{
	return get_asset<TextureAtlas>(name);
}

namespace
{
	const int kKvadVersion = 100;

	const char kKvadFailedLoading[] = "Kvad: Failed to load %s: %s";
	const char kKvadFailedOpening[] = "Kvad: Failed to open file: %s";
	const char kKvadNoSuchFile[] = "Kvad: No such file: %s";
#ifndef NDEBUG
	const char kKvadMissingProperty[] = "Kvad: Missing property '%s' on %s: %s";
	const char kKvadUnknownProperty[] = "Kvad: Unknown property '%s' on %s: %s";
#endif

	class ScopedField
	{
	public:
		ScopedField(lua_State* L, int n) : state_(L) { lua_rawgeti(L, -1, n); }

		ScopedField(lua_State* L, const char* s) : state_(L)
		{
			lua_pushstring(L, s);
			lua_rawget(L, -2);
		}

		ScopedField(lua_State* L, const char* s, size_t len) : state_(L)
		{
			lua_pushlstring(L, s, len);
			lua_rawget(L, -2);
		}

		~ScopedField() { lua_pop(state_, 1); }

	private:
		lua_State* state_;
	};

	template <size_t N>
	ScopedField get_field(lua_State* L, const char (&name)[N])
	{
		return ScopedField(L, name, N - 1);
	}

	template <size_t N>
	bool has_key(lua_State* L, const char (&name)[N])
	{
		if (!lua_istable(L, -1))
			return false;

		auto field = get_field(L, name);
		return !lua_isnil(L, -1);
	}

	Kvad::Asset no_asset() { return {Kvad::AssetType::None, nullptr, nullptr}; }

	const char* table_name(lua_State* L)
	{
		if (has_key(L, "name"))
		{
			auto field = get_field(L, "name");
			return lua_tostring(L, -1);
		}

		switch (lua_type(L, -2))
		{
			case LUA_TNUMBER: {
				const char* name =
				    lua_pushfstring(L, "#%d", lua_tointeger(L, -2));
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
	void parse_table(lua_State* L, const char* name, F&& parse, Args&&... args)
	{
		ScopedField table(L, name);
		R_ASSERT(lua_istable(L, -1), "Table expected");

		lua_pushnil(L);
		while (lua_next(L, -2) != 0)
		{
			auto asset = parse(L, std::forward<Args>(args)...);
			if (asset.type == Kvad::AssetType::None)
				LOGE("Kvad: Failed to parse '%s'", table_name(L));
			lua_pop(L, 1);
		}
	}

#include "Script/KvadNode.h"
#include "Script/KvadResource.h"

#if USE_NODE_TAGS
	std::unique_ptr<char[]> basename_without_extension(const char* path)
	{
		if (!path)
			return {};

		const char* i = path;
		const char* start = path;
		const char* end = path;
		while (*i)
		{
			if (*i == '.')
				end = i;
			else if (*i == '/' || *i == '\'')
				start = i + 1;
			++i;
		}
		if (end <= start)
			end = i;
		if (start >= end)
			return {};

		const ptrdiff_t length = end - start;
		auto name = std::make_unique<char[]>(length + 1);
		strncpy(name.get(), start, length);
		name[length] = '\0';
		return std::move(name);
	}
#endif

	size_t size_of(Kvad::AssetType type)
	{
		switch (type)
		{
			case Kvad::AssetType::Animation:
				return rainbow::ScopeStack::size_of<Animation>();
			case Kvad::AssetType::FontAtlas:
				return rainbow::ScopeStack::size_of<FontAtlas>();
			case Kvad::AssetType::Label:
				return rainbow::ScopeStack::size_of<Label>();
			case Kvad::AssetType::Node:
				return rainbow::ScopeStack::size_of<SceneNode>();
			case Kvad::AssetType::SpriteBatch:
				return rainbow::ScopeStack::size_of<SpriteBatch>();
			case Kvad::AssetType::Sound:
				return rainbow::ScopeStack::size_of<void*>();
			case Kvad::AssetType::TextureAtlas:
				return rainbow::ScopeStack::size_of<TextureAtlas>();
			default:
				return 0;
		}
	}

	Kvad::Asset compute_size(lua_State* L, size_t& total_size)
	{
		auto type = node_type(L);
		if (type == Kvad::AssetType::None)
			type = resource_type(L);
		else if (type == Kvad::AssetType::SpriteBatch)
			parse_table(L, "sprites", &compute_size, total_size);

		if (type == Kvad::AssetType::None)
		{
			if (has_key(L, "texture"))
			{
				type = Kvad::AssetType::Sprite;
				if (has_key(L, "animations"))
					parse_table(L, "animations", &compute_size, total_size);
			}
			else if (has_key(L, "fps"))
				type = Kvad::AssetType::Animation;
		}
		total_size += size_of(type);
		return {type, nullptr, nullptr};
	}
}

Kvad* Kvad::from_lua(const char* path)
{
	const Data script(File::open(path));
	if (!script)
		return nullptr;

	std::unique_ptr<lua_State, decltype(&lua_close)> L(luaL_newstate(),
	                                                   lua_close);
	lua_gc(L.get(), LUA_GCSTOP, 0);
	lua_createtable(L.get(), 0, 1);
	lua_pushliteral(L.get(), "platform");
	lua_createtable(L.get(), 0, 1);
	lua_pushliteral(L.get(), "screen");
	lua_createtable(L.get(), 0, 2);
	const Vec2i& screen = Renderer::Get()->resolution();
	luaR_rawsetnumber(L.get(), "width", screen.x);
	luaR_rawsetnumber(L.get(), "height", screen.y);
	lua_rawset(L.get(), -3);
	lua_rawset(L.get(), -3);
	lua_setglobal(L.get(), "rainbow");

	if (rainbow::lua::load(L.get(), script, "Kvad") == 0)
		return nullptr;

	if (!has_key(L.get(), "version"))
		R_ABORT("Kvad: Version required and missing");
	else
	{
		auto field = get_field(L.get(), "version");
		if (lua_tointeger(L.get(), -1) < kKvadVersion)
		{
			R_ABORT("Kvad: Version %i required", kKvadVersion);
			return nullptr;
		}
	}

	// Compute the memory requirement for this scene.
	size_t total_size = 0;
	parse_table(L.get(), "resources", &compute_size, total_size);
	parse_table(L.get(), "nodes", &compute_size, total_size);

	Kvad* scene = new Kvad(total_size);
	parse_table(
	    L.get(), "resources", &create_resource, scene->stack_, scene->assets_);
	parse_table(L.get(),
	            "nodes",
	            &create_node,
	            *scene,
	            scene->assets_,
	            scene->stack_,
	            scene->node_);

#if USE_NODE_TAGS
	if (const auto& name = basename_without_extension(path))
		scene->node()->set_tag(name.get());
#endif  // USE_NODE_TAGS

	return scene;
}
