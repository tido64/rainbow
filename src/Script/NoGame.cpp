// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/NoGame.h"

#include "Resources/Rainbow.svg.h"
#include "Script/Transition.h"

void NoGame::init(const Vec2i& screen)
{
    constexpr const float kDesiredHeight = 1440.0f;

    const float scale = screen.y / kDesiredHeight;
    const float logo_scale = 0.1f * scale;
    const unsigned int logo_width = kRainbowLogoWidth * logo_scale;
    const unsigned int logo_height = kRainbowLogoHeight * logo_scale;
    const unsigned int logo_padding = 32 * scale;

    batch_ = rainbow::spritebatch(1);
    auto texture = rainbow::texture(kRainbowLogoURI, kRainbowLogo, logo_scale);
    batch_->set_texture(texture);

    frame_ = batch_->create_sprite(
        logo_width + logo_padding, logo_height + logo_padding);
    frame_->set_color(0xffffff00);
    frame_->set_position(Vec2f(screen.x * 0.5f, screen.y * 0.5f));
    frame_->set_texture(texture->add_region(0, 64, 0, 0));

    auto logo = batch_->create_sprite(logo_width, logo_height);
    logo->set_color(0x000000ff);
    logo->set_position(Vec2f(screen.x * 0.5f, screen.y * 0.5f));
    logo->set_texture(texture->add_region(0, 0, logo_width, logo_height));

    scenegraph().add_child(batch_);
}

void NoGame::update(unsigned long)
{
    static bool run_once = false;
    if (!run_once)
    {
        run_once = true;
        rainbow::fade(frame_, 1.0f, 2000, &rainbow::timing::linear);
    }
}

std::unique_ptr<GameBase> GameBase::create(rainbow::Director& director)
{
    return std::make_unique<NoGame>(director);
}
