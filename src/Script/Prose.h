// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_PROSE_H_
#define SCRIPT_PROSE_H_

#include <string>
#include <unordered_map>

#include "Memory/ScopeStack.h"

class Animation;
class FontAtlas;
class Label;
class Sprite;
class SpriteBatch;
class TextureAtlas;

namespace rainbow { class SceneNode; }

class Prose
{
public:
    enum class AssetType;

    struct Asset
    {
        AssetType type;
        void* ptr;
        rainbow::SceneNode* node;
    };

    using AssetMap = std::unordered_map<std::string, Asset>;

    static auto from_lua(const char* path) -> Prose*;

    Prose(size_t size);
    ~Prose();

    auto node() { return node_; }

    template <typename T>
    auto get_asset(const std::string& name) -> T*;

    auto get_animation(const std::string& name) -> Animation*;
    auto get_font(const std::string& name) -> FontAtlas*;
    auto get_label(const std::string& name) -> Label*;
    auto get_node(const std::string& name) -> rainbow::SceneNode*;
    auto get_sprite(const std::string& name) -> Sprite*;
    auto get_spritebatch(const std::string& name) -> SpriteBatch*;
    auto get_texture(const std::string& name) -> TextureAtlas*;

private:
    AssetMap assets_;
    rainbow::LinearAllocator allocator_;
    rainbow::ScopeStack stack_;
    rainbow::SceneNode* node_;

    template <typename T, Prose::AssetType Type>
    auto get_asset(const std::string& name) -> T*;
};

#endif
