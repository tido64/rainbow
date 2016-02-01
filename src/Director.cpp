// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Director.h"

#include <lua.hpp>

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
    Director::Director() : active_(true), terminated_(false), error_(nullptr)
    {
        if (!mixer_.initialize(kMaxAudioChannels))
            terminate("Failed to initialise audio engine");
        else if (!renderer_.init())
            terminate("Failed to initialise renderer");
#if USE_NODE_TAGS
        scenegraph_.set_tag("root");
#endif
    }

    Director::~Director()
    {
        // Clean up before we tear down the graphics context.
        script_.reset();
    }

    void Director::draw()
    {
        Renderer::clear();
        scenegraph_.draw();
#ifdef USE_PHYSICS
        b2::DebugDraw::Draw();
#endif  // USE_PHYSICS
    }

    void Director::init(const Vec2i& screen)
    {
        random.seed();
        renderer_.set_resolution(screen);
        script_.reset(GameBase::create(*this));
        script_->init(screen);

        if (terminated())
            return;

        scenegraph_.update(0);
    }

    void Director::update(unsigned long dt)
    {
        R_ASSERT(!terminated_, "App should have terminated by now");

        mixer_.process();
        timer_manager_.update(dt);
        script_->update(dt);
        scenegraph_.update(dt);
        TextureManager::Get()->trim();
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
