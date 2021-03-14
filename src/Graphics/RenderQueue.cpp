// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/RenderQueue.h"

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

using rainbow::Animation;
using rainbow::GameBase;
using rainbow::IDrawable;
using rainbow::Label;
using rainbow::SpriteBatch;
using rainbow::graphics::Context;
using rainbow::graphics::RenderQueue;

namespace
{
    struct DrawCommand {
        Context& context;  // NOLINT

        void operator()(Animation*) const {}

        void operator()(IDrawable* drawable) const { drawable->draw(context); }

        template <typename T>
        void operator()(T&& unit) const
        {
            rainbow::graphics::draw(context, *unit);
        }
    };

    struct UpdateCommand {
        GameBase& context;  // NOLINT
        const uint64_t dt;  // NOLINT

        void operator()(Animation* animation) const { animation->update(dt); }

        void operator()(Label* label) const { label->update(context); }

        void operator()(SpriteBatch* batch) const { batch->update(context); }

        template <typename T>
        void operator()(T&& unit) const
        {
            unit->update(context, dt);
        }
    };
}  // namespace

void rainbow::graphics::draw(Context& ctx, RenderQueue& queue)
{
    visit_all(DrawCommand{ctx}, queue);
}

void rainbow::graphics::update(GameBase& ctx, RenderQueue& queue, uint64_t dt)
{
    visit_all(UpdateCommand{ctx, dt}, queue);
}
