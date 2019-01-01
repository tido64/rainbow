// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/RenderQueue.h"

#include "Graphics/Animation.h"
#include "Graphics/Drawable.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"

using rainbow::Animation;
using rainbow::Label;
using rainbow::SpriteBatch;
using rainbow::graphics::RenderNode;
using rainbow::graphics::RenderQueue;

namespace vk = rainbow::vk;

namespace
{
    struct DrawCommand
    {
        const vk::CommandBuffer& command_buffer;
        const vk::IndexBuffer& index_buffer;

        void operator()(Animation*) const {}

        void operator()(Label* label) const
        {
            rainbow::vk::draw(command_buffer, *label, index_buffer);
        }

        void operator()(SpriteBatch* sprite_batch) const
        {
            rainbow::vk::draw(command_buffer, *sprite_batch, index_buffer);
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
}  // namespace

#ifndef NDEBUG
using namespace std::literals::string_view_literals;

namespace rainbow::graphics
{
    std::vector<RenderUnit> g_deleted_units;
}

namespace
{
    struct LifetimeValidator
    {
        ~LifetimeValidator() { rainbow::graphics::g_deleted_units.clear(); }

        void validate(const RenderNode& node) const
        {
            const auto is_alive = std::none_of(  //
                std::begin(rainbow::graphics::g_deleted_units),
                std::end(rainbow::graphics::g_deleted_units),
                [unit = node.unit()](auto&& deleted) {
                    return unit == deleted;
                });
            R_ASSERT(is_alive, visit(*this, node.unit()).data());
        }

        auto operator()(Animation*) const -> std::string_view
        {
            return "Animation deleted but is still in the render queue."sv;
        }

        auto operator()(Label*) const -> std::string_view
        {
            return "Label deleted but is still in the render queue."sv;
        }

        auto operator()(SpriteBatch*) const -> std::string_view
        {
            return "SpriteBatch deleted but is still in the render queue."sv;
        }

        template <typename T>
        auto operator()(T&&) const -> std::string_view
        {
            return "Drawable deleted but is still in the render queue."sv;
        }
    };
}  // namespace
#endif

void rainbow::graphics::draw(const vk::CommandBuffer& command_buffer,
                             const RenderQueue& queue,
                             const vk::IndexBuffer& index_buffer)
{
    IF_DEBUG(LifetimeValidator lifetime_validator);
    const DrawCommand draw{command_buffer, index_buffer};
    for (auto&& node : queue)
    {
        IF_DEBUG(lifetime_validator.validate(node));
        if (!node.is_enabled())
            continue;

        visit(draw, node.unit());
    }
}

void rainbow::graphics::update(RenderQueue& queue, uint64_t dt)
{
    for (auto&& node : queue)
    {
        if (!node.is_enabled())
            continue;

        visit(UpdateCommand{dt}, node.unit());
    }
}
