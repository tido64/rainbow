// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Audio/Mixer.h"
#include "Common/Global.h"
#include "Graphics/RenderQueue.h"
#include "Graphics/Renderer.h"
#include "Input/Input.h"
#include "Script/Timer.h"
#include "Text/Typesetter.h"

namespace rainbow
{
    class Data;
    class GameBase;

    /// <summary>
    ///   Simple game loop. Must be created after having set up a video context.
    /// </summary>
    class Director final
        : IF_DEBUG_ELSE(public Global<Director>, private NonCopyable<Director>)
    {
    public:
        Director();
        ~Director();

        /// <summary>Loads and initialises main script.</summary>
        void init(const Vec2i& screen);

        bool active() const { return active_; }
        auto error() const { return error_; }
        auto font_cache() -> FontCache& { return typesetter_.font_cache(); }
        auto graphics_context() -> graphics::State& { return renderer_; }
        auto input() -> Input& { return input_; }
        auto mixer() -> audio::Mixer& { return mixer_; }
        auto render_queue() -> graphics::RenderQueue& { return render_queue_; }
        auto script() { return script_.get(); }
        bool terminated() const { return terminated_; }

        auto texture_manager() -> graphics::TextureManager&
        {
            return renderer_.texture_manager;
        }

        void draw();
        void restart();

        void terminate()
        {
            active_ = false;
            terminated_ = true;
        }

        void terminate(std::error_code error)
        {
            terminate();
            error_ = error;
        }

        /// <summary>Updates world.</summary>
        /// <param name="dt">Milliseconds since last frame.</param>
        void update(uint64_t dt);

        void on_focus_gained();
        void on_focus_lost();

        /// <summary>Called when a low memory warning has been issued.</summary>
        void on_memory_warning();

#ifndef NDEBUG
        template <typename T>
        static void assert_unused(T* ptr, czstring reason)
        {
            auto director = Get();
            if (director == nullptr)
                return;

            auto& render_queue = director->render_queue();
            auto is_same_object = [ptr](auto&& unit) {
                return get<T*>(unit.object()) == ptr;
            };
            R_ASSERT(
                director->terminated() || std::none_of(std::begin(render_queue),
                                                       std::end(render_queue),
                                                       is_same_object),
                reason);
        }
#endif

    private:
        bool active_;
        bool terminated_;
        std::error_code error_;
        TimerManager timer_manager_;
        std::unique_ptr<GameBase> script_;
        graphics::RenderQueue render_queue_;
        Input input_;
        graphics::State renderer_;
        audio::Mixer mixer_;
        Typesetter typesetter_;

        void start();
    };
}  // namespace rainbow

#ifdef USE_HEIMDALL
#   include "Heimdall/Gatekeeper.h"
#else
using Director = rainbow::Director;
#endif

#endif
