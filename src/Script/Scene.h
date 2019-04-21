// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_SCENE_H_
#define SCRIPT_SCENE_H_

#include <string>

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#    pragma GCC diagnostic ignored "-Woverflow"
#    ifdef __clang__
#        pragma clang diagnostic ignored "-Wcomma"
#    endif
#endif
#include <absl/container/flat_hash_map.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif
#include <duktape.h>

#include "Common/Passkey.h"
#include "Common/String.h"
#include "Common/Variant.h"
#include "Graphics/Drawable.h"
#include "Graphics/RenderQueue.h"
#include "Memory/ScopeStack.h"

namespace rainbow
{
    class TextureAtlas;

    class Scene final : public IDrawable
    {
        using asset_type = variant<  //
            Animation*,
            Label*,
            SpriteBatch*,
            TextureAtlas*>;

    public:
        static auto from_json(duk_context* ctx, czstring path)
            -> std::unique_ptr<Scene>;

        Scene(duk_context* ctx, size_t size, const Passkey<Scene>&);

        template <typename T>
        auto get(const std::string& key) -> T*
        {
            R_ASSERT(!key.empty(), "Empty keys are invalid");

            auto i = asset_map_.find(key);
            return i == asset_map_.end()
                       ? nullptr
                       : rainbow::get<T*>(i->second).value_or(nullptr);
        }

    private:
        absl::flat_hash_map<std::string, asset_type> asset_map_;
        LinearAllocator allocator_;
        ScopeStack stack_;
        graphics::RenderQueue render_queue_;

        void store_asset(duk_context* ctx, asset_type ptr);

        // IDrawable implementation details.

        void draw_impl() override;
        void update_impl(uint64_t dt) override;
    };
}  // namespace rainbow

#endif
