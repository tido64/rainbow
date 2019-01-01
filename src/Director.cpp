// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Director.h"

#include "Common/Logging.h"
#include "Common/Random.h"
#include "Graphics/Driver.h"
#include "Script/NoGame.h"

#ifdef USE_PHYSICS
#    include "ThirdParty/Box2D/DebugDraw.h"
#endif  // USE_PHYSICS

namespace
{
    constexpr int kMaxAudioChannels = 24;
}

namespace rainbow
{
    Random random;
}

rainbow::Director::Director(const graphics::Driver& driver)
    : texture_manager_(driver.device()), typesetter_(texture_manager_),
      command_buffer_(driver.swapchain(), driver.default_pipeline()),
      index_buffer_(driver.device())
{
    if (std::error_code error = mixer_.initialize(kMaxAudioChannels))
        terminate(error);
}

rainbow::Director::~Director()
{
    // Clean up before we tear down the graphics context.
    script_.reset();
    render_queue_.clear();
}

void rainbow::Director::init(Vec2i)
{
    random.seed();
    start();
}

void rainbow::Director::draw()
{
    command_buffer_.begin();
    graphics::draw(command_buffer_, render_queue_, index_buffer_);
    command_buffer_.end();
#ifdef USE_PHYSICS
    b2::DebugDraw::Draw();
#endif  // USE_PHYSICS
}

void rainbow::Director::restart()
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

void rainbow::Director::update(uint64_t dt)
{
    R_ASSERT(!terminated_, "App should have terminated by now");

    timer_manager_.update(dt);
    script_->update(dt);

    mixer_.process();
    graphics::update(render_queue_, dt);
    font_cache().update(texture_manager_.device());
}

void rainbow::Director::on_focus_gained()
{
    R_ASSERT(!terminated_, "App should have terminated by now");

    active_ = true;
    mixer_.suspend(false);
}

void rainbow::Director::on_focus_lost()
{
    active_ = false;
    mixer_.suspend(true);
}

void rainbow::Director::on_memory_warning()
{
    R_ASSERT(!terminated_, "App should have terminated by now");

    script_->on_memory_warning();
}

void rainbow::Director::start()
{
    script_ = GameBase::create(*this);
    if (!terminated())
        script_->init(surface_size_);

    if (terminated())
    {
        LOGF("%s", error().message().c_str());
        LOGI("Booting 'NoGame'...");
        script_ = std::make_unique<NoGame>(*this, error());
        active_ = true;
        terminated_ = false;
        script_->init(surface_size_);
    }

    graphics::update(render_queue_, 0);
}
