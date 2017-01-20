// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_PROSE_H_
#define SCRIPT_PROSE_H_

#include <string>
#include <unordered_map>

#include "Graphics/Drawable.h"
#include "Graphics/RenderQueue.h"
#include "Memory/ScopeStack.h"

namespace rainbow
{
    class Animation;
    class FontAtlas;
    class Label;
    class Sprite;
    class SpriteBatch;
    class TextureAtlas;

    class Prose final : public IDrawable
    {
    public:
        enum class AssetType;

        struct Asset
        {
            AssetType type;
            void* ptr;
            uint32_t id;

            static auto none() -> Asset;
        };

        using AssetMap = std::unordered_map<std::string, Asset>;

        static auto from_lua(const char* path) -> Prose*;

        Prose(size_t size);

        template <typename T>
        auto get_asset(const std::string& name) -> T*;

        auto get_animation(const std::string& name) -> Animation*;
        auto get_font(const std::string& name) -> FontAtlas*;
        auto get_label(const std::string& name) -> Label*;
        auto get_sprite(const std::string& name) -> Sprite*;
        auto get_spritebatch(const std::string& name) -> SpriteBatch*;
        auto get_texture(const std::string& name) -> TextureAtlas*;

    private:
        AssetMap assets_;
        LinearAllocator allocator_;
        ScopeStack stack_;
        graphics::RenderQueue render_queue_;

        template <typename T, AssetType Type>
        auto get_asset(const std::string& name, const char* error) -> T*;

        void draw_impl() override
        {
            graphics::draw(render_queue_);
        }

        void update_impl(uint64_t dt) override
        {
            graphics::update(render_queue_, dt);
        }
    };
}

#endif
