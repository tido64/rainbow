// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Director.h"

#include "Common/Random.h"
#include "Script/GameBase.h"

#ifdef USE_PHYSICS
#   include "ThirdParty/Box2D/DebugDraw.h"
#endif  // USE_PHYSICS

namespace
{
    constexpr int kMaxAudioChannels = 24;
}

namespace rainbow
{
    Random random;

    Director::Director() : active_(true), terminated_(false), error_(nullptr)
    {
        if (!mixer_.initialize(kMaxAudioChannels))
            terminate("Failed to initialise audio engine");
        else if (!renderer_.initialize())
            terminate("Failed to initialise renderer");
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
        graphics::set_resolution(screen);

        script_ = GameBase::create(*this);
        if (terminated())
            return;

        script_->init(screen);
        if (terminated())
            return;

        graphics::update(render_queue_, 0);
    }

    void Director::draw()
    {
        graphics::clear();
        graphics::draw(render_queue_);
#ifdef USE_PHYSICS
        b2::DebugDraw::Draw();
#endif  // USE_PHYSICS
    }

    void Director::update(uint64_t dt)
    {
        R_ASSERT(!terminated_, "App should have terminated by now");

        mixer_.process();
        timer_manager_.update(dt);
        script_->update(dt);
        graphics::update(render_queue_, dt);
        graphics::TextureManager::Get()->trim();
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
}
