// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/NoGame.h"

#include "Resources/Rainbow.svg.h"
#include "Script/Transition.h"

using rainbow::GameBase;
using rainbow::Sprite;
using rainbow::Vec2f;
using rainbow::Vec2i;

void NoGame::init_impl(const Vec2i& screen)
{
    constexpr float kDesiredHeight = 1440.0f;

    const float scale = screen.y / kDesiredHeight;
    const float logo_scale = 0.1f * scale;
    const unsigned int logo_width = kRainbowLogoWidth * logo_scale;
    const unsigned int logo_height = kRainbowLogoHeight * logo_scale;

    auto texture = rainbow::texture(
        kRainbowLogoURI,
        kRainbowLogo,
        logo_scale,
        std::make_tuple(0, 64, 0, 0),                     // 0
        std::make_tuple(0, 0, logo_width, logo_height));  // 1

    const unsigned int logo_padding = 32 * scale;
    const Vec2f center{screen.x * 0.5f, screen.y * 0.5f};

    batch_ = rainbow::spritebatch(
        texture,
        Sprite{logo_width + logo_padding, logo_height + logo_padding}  // 0
            .position(center)
            .texture(0)
            .color(0xffffff00),
        Sprite{logo_width, logo_height}  // 1
            .position(center)
            .texture(1)
            .color(0x000000ff));

    scenegraph().add_child(batch_);
}

void NoGame::update_impl(uint64_t)
{
    static bool run_once = false;
    if (!run_once)
    {
        run_once = true;
        rainbow::fade(&batch_->at(0), 1.0f, 2000, &rainbow::timing::linear);
    }
}

auto GameBase::create(rainbow::Director& director) -> std::unique_ptr<GameBase>
{
    return std::make_unique<NoGame>(director);
}
