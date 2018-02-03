// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_NOGAME_H_
#define SCRIPT_NOGAME_H_

#include "Graphics/SpriteBatch.h"
#include "Script/GameBase.h"

namespace rainbow
{
    class NoGame final : public GameBase
    {
    public:
        NoGame(Director& director) : GameBase(director) {}

    private:
        std::unique_ptr<SpriteBatch> batch_;

        void init_impl(const Vec2i& screen) override;
        void update_impl(uint64_t dt) override;
    };
}

#endif
