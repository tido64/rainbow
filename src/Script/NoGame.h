// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_NOGAME_H_
#define SCRIPT_NOGAME_H_

#include "Script/GameBase.h"

class NoGame final : public rainbow::GameBase
{
public:
    NoGame(rainbow::Director& director) : rainbow::GameBase(director) {}

private:
    rainbow::spritebatch_t batch_;

    void init_impl(const rainbow::Vec2i& screen) override;
    void update_impl(uint64_t dt) override;
};

#endif
