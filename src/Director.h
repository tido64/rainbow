// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "Audio/Mixer.h"
#include "Graphics/RenderQueue.h"
#include "Graphics/TextureManager.h"
#include "Input/Input.h"
#include "Script/Timer.h"
#include "Text/Typesetter.h"

namespace rainbow::graphics
{
    class Driver;
}

namespace rainbow
{
    class Data;
    class GameBase;

    /// <summary>
    ///   Simple game loop. Must be created after having set up a video context.
    /// </summary>
    class Director final : NonCopyable<Director>
    {
    public:
        Director(const graphics::Driver& driver);
        ~Director();

        /// <summary>Loads and initialises main script.</summary>
        void init(Vec2i screen);

        bool active() const { return active_; }
        auto error() const { return error_; }
        auto font_cache() -> FontCache& { return typesetter_.font_cache(); }
        auto input() -> Input& { return input_; }
        auto mixer() -> audio::Mixer& { return mixer_; }
        auto render_queue() -> graphics::RenderQueue& { return render_queue_; }
        auto script() { return script_.get(); }
        bool terminated() const { return terminated_; }

        auto texture_manager() -> graphics::TextureManager&
        {
            return texture_manager_;
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

    private:
        bool active_ = true;
        bool terminated_ = false;
        std::error_code error_ = ErrorCode::Success;
        TimerManager timer_manager_;
        std::unique_ptr<GameBase> script_;
        Input input_;
        audio::Mixer mixer_;
        graphics::RenderQueue render_queue_;
        graphics::TextureManager texture_manager_;
        Typesetter typesetter_;
        vk::CommandBuffer command_buffer_;
        vk::IndexBuffer index_buffer_;
        Vec2i surface_size_;

        void start();
    };
}  // namespace rainbow

#ifdef USE_HEIMDALL
#    include "Heimdall/Gatekeeper.h"
#else
using Director = rainbow::Director;
#endif

#endif
