// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_GAMEBASE_H_
#define SCRIPT_GAMEBASE_H_

#include "Director.h"

namespace rainbow
{
    class GameBase
    {
    public:
        static auto create(Director& director) -> std::unique_ptr<GameBase>;

        GameBase(const GameBase&) = default;
        GameBase(GameBase&&) noexcept = default;
        virtual ~GameBase() = default;

        [[nodiscard]] auto input() -> Input& { return director_.input(); }

        [[nodiscard]] auto render_queue() -> graphics::RenderQueue&
        {
            return director_.render_queue();
        }

        [[nodiscard]] auto texture_provider() -> graphics::TextureProvider&
        {
            return director_.texture_provider();
        }

        [[nodiscard]] auto texture_provider() const
            -> graphics::TextureProvider&
        {
            return director_.texture_provider();
        }

        [[nodiscard]] auto timer_manager() -> TimerManager&
        {
            return director_.timer_manager();
        }

        [[nodiscard]] auto typesetter() -> Typesetter&
        {
            return director_.typesetter();
        }

        void init(const Vec2i& screen_size) { init_impl(screen_size); }
        void update(uint64_t dt) { update_impl(dt); }

        void terminate() { director_.terminate(); }
        void terminate(std::error_code error) { director_.terminate(error); }

        void on_memory_warning() { on_memory_warning_impl(); }

        auto operator=(const GameBase&) -> GameBase& = delete;
        auto operator=(GameBase&&) noexcept -> GameBase& = delete;

    protected:
        GameBase(Director& director) : director_(director) {}

    private:
        Director& director_;

        virtual void init_impl(const Vec2i&) {}
        virtual void update_impl(uint64_t) {}
        virtual void on_memory_warning_impl() {}
    };
}  // namespace rainbow

#endif
