// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Director.h"

#include "Common/Logging.h"
#include "Common/Random.h"
#include "Script/NoGame.h"

#ifdef USE_PHYSICS
#    include "ThirdParty/Box2D/DebugDraw.h"
#endif  // USE_PHYSICS

namespace
{
    constexpr int kMaxAudioChannels = 24;
}  // namespace

namespace rainbow
{
    Random random;  // NOLINT(cert-err58-cpp)

    Director::Director()
        : active_(true), terminated_(false), error_(ErrorCode::Success)
    {
        if (std::error_code error = mixer_.initialize(kMaxAudioChannels))
            terminate(error);
        else if (std::error_code error = renderer_.initialize())
            terminate(error);

        IF_DEBUG(make_global());
    }

    Director::~Director()
    {
        // Clean up before we tear down the graphics context.
        script_.reset();
        render_queue_.clear();
    }

    void Director::init(const Vec2i& screen)
    {
        random.seed();
        graphics::set_surface_size(renderer_, screen);
        start();
    }

    void Director::draw()
    {
        graphics::clear();
        graphics::draw(render_queue_);
#ifdef USE_PHYSICS
        b2::DebugDraw::Draw();
#endif  // USE_PHYSICS
    }

    void Director::restart()
    {
        terminate();

        script_.reset();
        timer_manager_.clear();
        render_queue_.clear();
        mixer_.clear();

        active_ = true;
        terminated_ = false;
        error_ = ErrorCode::Success;

        start();
    }

    void Director::update(uint64_t dt)
    {
        R_ASSERT(!terminated_, "App should have terminated by now");

        timer_manager_.update(dt);
        script_->update(dt);

        graphics::update(render_queue_, dt);
        font_cache().update(renderer_.texture_manager);
        renderer_.texture_manager.trim();

        mixer_.process();
    }

    void Director::on_focus_gained()
    {
        R_ASSERT(!terminated_, "App should have terminated by now");

        active_ = true;
        mixer_.suspend(false);
    }

    void Director::on_focus_lost()
    {
        active_ = false;
        mixer_.suspend(true);
    }

    void Director::on_memory_warning()
    {
        R_ASSERT(!terminated_, "App should have terminated by now");

        script_->on_memory_warning();
    }

    void Director::start()
    {
        script_ = GameBase::create(*this);
        if (!terminated())
            script_->init(renderer_.surface_size);

        if (terminated())
        {
            LOGF("%s", error().message().c_str());
            LOGI("Booting 'NoGame'...");
            script_ = std::make_unique<NoGame>(*this, error());
            active_ = true;
            terminated_ = false;
            script_->init(renderer_.surface_size);
        }

        graphics::update(render_queue_, 0);
    }
}  // namespace rainbow
