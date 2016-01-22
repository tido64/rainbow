// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/NoGame.h"
#include "Script/Transition.h"

void NoGame::init(const Vec2i& screen)
{
    batch_ = rainbow::spritebatch(1);
    auto texture = rainbow::texture("Rainbow.png");
    batch_->set_texture(texture);
    logo_ = batch_->create_sprite(392, 710);
    logo_->set_color(Colorb(0xffffff00));
    logo_->set_position(Vec2f(screen.x * 0.5f, screen.y * 0.5f));
    logo_->set_scale(0.5);
    logo_->set_texture(texture->add_region(1, 1, 392, 710));
    scenegraph().add_child(batch_);
}

void NoGame::update(unsigned long)
{
    static bool run_once = false;
    if (!run_once)
    {
        run_once = true;
        rainbow::fade(logo_, 1.0f, 1500, &rainbow::timing::linear);
    }
}

GameBase* GameBase::create(rainbow::Director& director)
{
    return new NoGame(director);
}
