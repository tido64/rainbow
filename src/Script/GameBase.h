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

        virtual ~GameBase() {}

        auto input() -> Input& { return director_.input(); }

        auto render_queue() -> graphics::RenderQueue&
        {
            return director_.render_queue();
        }

        void terminate() { director_.terminate(); }
        void terminate(std::error_code error) { director_.terminate(error); }

        void init(const Vec2i& screen_size) { init_impl(screen_size); }
        void update(uint64_t dt) { update_impl(dt); }
        void on_memory_warning() { on_memory_warning_impl(); }

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
