// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/RenderQueue.h"

#include "Platform/Macros.h"
#if HAS_EXECUTION_POLICY
#   include <execution>
#endif

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteBatch.h"
#include "Text/FontCache.h"

using rainbow::Animation;
using rainbow::FontCache;
using rainbow::Label;
using rainbow::SpriteBatch;
using rainbow::graphics::RenderQueue;

namespace
{
    struct DrawCommand
    {
        void operator()(Animation*) const {}

        void operator()(Label* label) const
        {
            FontCache::Get()->bind_texture();
            rainbow::graphics::draw(*label);
        }

        void operator()(SpriteBatch* sprite_batch) const
        {
            rainbow::graphics::draw(*sprite_batch);
        }

        template <typename T>
        void operator()(T&& unit) const
        {
            unit->draw();
        }
    };

    struct UpdateCommand
    {
        const uint64_t dt;

        void operator()(Label* label) const { label->update(); }

        void operator()(SpriteBatch* sprite_batch) const
        {
            sprite_batch->update();
        }

        template <typename T>
        void operator()(T&& unit) const
        {
            unit->update(dt);
        }
    };

    struct UploadCommand
    {
        void operator()(Animation*) const {}

        template <typename T>
        void operator()(T&& unit) const
        {
            unit->upload();
        }
    };
}  // namespace

void rainbow::graphics::draw(RenderQueue& queue)
{
    for (auto&& unit : queue)
    {
        if (!unit.is_enabled())
            continue;

        visit(DrawCommand{}, unit.object());
    }
}

void rainbow::graphics::update(RenderQueue& queue, uint64_t dt)
{
    for (auto&& unit : queue)
    {
        if (!unit.is_enabled())
            continue;

        visit(UpdateCommand{dt}, unit.object());
        visit(UploadCommand{}, unit.object());
    }
}

void rainbow::graphics::update_parallel(RenderQueue& queue, uint64_t dt)
{
#if HAS_EXECUTION_POLICY
    static std::vector<RenderUnit::variant_type> uploads;

    uploads.resize(queue.size());
    std::transform(std::execution::par_unseq,
                   queue.begin(),
                   queue.end(),
                   uploads.begin(),
                   [dt](auto&& unit) -> RenderUnit::variant_type {
                       if (!unit.is_enabled())
                           return static_cast<Animation*>(nullptr);

                       visit(UpdateCommand{dt}, unit.object());
                       return unit.object();
                   });
    std::for_each(std::begin(uploads), std::end(uploads), [](auto&& unit) {
        visit(UploadCommand{}, unit);
    });
#else
    update(queue, dt);
#endif
}
