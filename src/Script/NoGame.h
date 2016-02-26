// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_NOGAME_H_
#define SCRIPT_NOGAME_H_

#include "Script/GameBase.h"

class NoGame final : public GameBase
{
public:
    NoGame(rainbow::Director& director) : GameBase(director) {}

    void init(const Vec2i& screen) override;
    void update(unsigned long dt) override;

private:
    rainbow::sprite_t frame_;
    rainbow::spritebatch_t batch_;
};

#endif
