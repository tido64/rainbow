// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_GAMEBASE_H_
#define SCRIPT_GAMEBASE_H_

#include "Director.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

namespace rainbow
{
    class GameBase
    {
    public:
        static auto create(Director& director) -> std::unique_ptr<GameBase>;

        GameBase(Director& director) : director_(director) {}
        virtual ~GameBase() = default;

        //
        // System
        //

        auto input() -> Input& { return director_.input(); }

        auto render_queue() -> graphics::RenderQueue&
        {
            return director_.render_queue();
        }

        auto texture_manager() -> graphics::TextureManager&
        {
            return director_.graphics_context().texture_manager;
        }

        //
        // Factory
        //

        auto make_label() { return std::make_unique<Label>(driver_); }

        auto make_label(void* ptr) { return new (ptr) Label{driver_}; }

        auto make_spritebatch(uint32_t count)
        {
            return std::make_unique<SpriteBatch>(driver_, count);
        }

        auto make_spritebatch(void* ptr, uint32_t count)
        {
            return new (ptr) SpriteBatch{driver_, count};
        }

        template <typename... Args>
        auto make_spritebatch(SharedPtr<TextureAtlas> texture,
                              Args&&... sprites)
        {
            return std::make_unique<SpriteBatch>(
                driver_, std::move(texture), std::forward<Args>(sprites)...);
        }

        //
        // Runtime
        //

        void init(const Vec2i& screen_size) { init_impl(screen_size); }

        void terminate() { director_.terminate(); }
        void terminate(std::error_code error) { director_.terminate(error); }

        void update(uint64_t dt) { update_impl(dt); }

        void on_memory_warning() { on_memory_warning_impl(); }

    private:
        Director& director_;

        virtual void init_impl(const Vec2i& screen_size) = 0;
        virtual void update_impl(uint64_t dt) = 0;
        virtual void on_memory_warning_impl() {}
    };
}  // namespace rainbow

#endif
